#include <iostream>
#include <string>
#include <string.h>

#include <mutex>
#include <thread>
#include <future>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include "logger/FileLogger.h"
#include "logger/LoggerFactory.h"

//TODO:
//bool RunAsDaemon();
//void handleSignal(int intSignal);

namespace daproxy {
namespace ip = boost::asio::ip;

class DAProxy : public boost::enable_shared_from_this<DAProxy> {
  public:

    typedef ip::tcp::socket socket_type;
    typedef boost::shared_ptr<DAProxy> ptr_type;

    DAProxy(boost::asio::io_service& ios)
        : downstream_socket_(ios),
          upstream_socket_(ios),
          sql_pattern_(boost::regex("^(INSERT|UPDATE|SELECT|WITH|DELETE|CALL|CREATE|ALTER|TRUNCATE|BEGIN|COMMIT|ROLLBACK)(?:[^;']|(?:'[^']+'))+\\s*$",
                                    boost::regex::icase)) {

        std::string log_dir {"/tmp/daproxy/"};
        std::string main_logger{"/tmp/daproxy/$(DATE)-daproxy_data.log"};
        std::string error_logger{"/tmp/daproxy/$(DATE)-daproxy_error.log"};

        boost::filesystem::path dir("path");

        if(!(boost::filesystem::exists(log_dir))) {

            if (boost::filesystem::create_directory(log_dir))
                cout << log_dir << " directory was successfully created for logging!" << endl;
        }

        LoggerFactory::Init(main_logger, error_logger, "I");
        main_logger_ = LoggerFactory::GetMainLogger();
        error_logger_ = LoggerFactory::GetErrorLogger();

    }

    socket_type& DownstreamSocket() {
        return downstream_socket_;
    }

    socket_type& UpstreamSocket() {
        return upstream_socket_;
    }

    void Start(const std::string& upstream_host, unsigned short upstream_port) {
        stringstream ss;
        ss << "[Connecting to " << upstream_host << ":" << upstream_port << "]";
        future_ = std::async( std::launch::async, &daproxy::DAProxy::LogInfo, this, ss.str());

        upstream_socket_.async_connect(
            ip::tcp::endpoint(
                boost::asio::ip::address::from_string(upstream_host),
                upstream_port),
            boost::bind(&DAProxy::HandleUpstreamConnect,
                        shared_from_this(),
                        boost::asio::placeholders::error));


    }
    void LogInfo(const string &message) {
        main_logger_->Info(message.c_str());
    }
    void HandleUpstreamConnect(const boost::system::error_code& error) {
        if (!error) {
            upstream_socket_.async_read_some(
                boost::asio::buffer(upstream_data_,max_data_length_),
                boost::bind(&DAProxy::HandleUpstreamRead,
                            shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));

            downstream_socket_.async_read_some(
                boost::asio::buffer(downstream_data_,max_data_length_),
                boost::bind(&DAProxy::HandleDownstreamRead,
                            shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
            future_ = std::async( std::launch::async, &daproxy::DAProxy::LogInfo, this, "[Connected]");
        } else
            Close();
    }

  private:

    Logger * main_logger_;
    Logger * error_logger_;
    std::future<void> future_;

    void HandleDownstreamWrite(const boost::system::error_code& error) {
        if (!error) {
            upstream_socket_.async_read_some(
                boost::asio::buffer(upstream_data_,max_data_length_),
                boost::bind(&DAProxy::HandleUpstreamRead,
                            shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
        } else
            Close();
    }

    void HandleDownstreamRead(const boost::system::error_code& error,
                              const size_t& bytes_transferred) {
        if (!error) {
            string message = GetDataMessage();

            if (message.length()>0)
                future_ = std::async( std::launch::async, &daproxy::DAProxy::LogInfo, this, message.c_str());

            async_write(upstream_socket_,
                        boost::asio::buffer(downstream_data_,bytes_transferred),
                        boost::bind(&DAProxy::HandleUpstreamWrite,
                                    shared_from_this(),
                                    boost::asio::placeholders::error));
        } else
            Close();
    }

    void HandleUpstreamWrite(const boost::system::error_code& error) {
        if (!error) {
            downstream_socket_.async_read_some(
                boost::asio::buffer(downstream_data_,max_data_length_),
                boost::bind(&DAProxy::HandleDownstreamRead,
                            shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
        } else
            Close();
    }

    void HandleUpstreamRead(const boost::system::error_code& error,
                            const size_t& bytes_transferred) {
        if (!error) {
            async_write(downstream_socket_,
                        boost::asio::buffer(upstream_data_,bytes_transferred),
                        boost::bind(&DAProxy::HandleDownstreamWrite,
                                    shared_from_this(),
                                    boost::asio::placeholders::error));
        } else
            Close();
    }

    void Close() {
        std::lock_guard<std::mutex> mlock(mutex_);

        if (downstream_socket_.is_open()) {
            downstream_socket_.close();
        }

        if (upstream_socket_.is_open()) {
            upstream_socket_.close();
        }
    }
    const string GetDataMessage() {
        string message = "";
        int packet_length = 0;
        try {
            packet_length = (downstream_data_[2] << 8) | (downstream_data_[1] << 8) | downstream_data_[0];
            if (packet_length > 4) {
                std::string data_string = std::string( downstream_data_ + protocol_header_, downstream_data_ + protocol_header_ + packet_length - 1);
                if (boost::regex_match(data_string,  sql_pattern_))
                    message = data_string;
            }
        } catch (std::exception ex) {
            error_logger_->Error(ex.what());
        } catch(...) {
        }

        return message;
    }

    socket_type downstream_socket_;
    socket_type upstream_socket_;
    enum { max_data_length_ = 8192 };
    const unsigned int protocol_header_ = 5;
    unsigned char downstream_data_[max_data_length_];
    unsigned char upstream_data_[max_data_length_];
    boost::regex sql_pattern_;
    std::mutex mutex_;

  public:

    class Acceptor {
      public:

        Acceptor(boost::asio::io_service& io_service,
                 const std::string& local_host, unsigned short local_port,
                 const std::string& upstream_host, unsigned short upstream_port)
            : _io_service(io_service),
              _localhost_address(boost::asio::ip::address_v4::from_string(local_host)),
              acceptor_(_io_service,ip::tcp::endpoint(_localhost_address,local_port)),
              upstream_port_(upstream_port),
              upstream_host_(upstream_host) {
        }

        bool AcceptConnections() {
            try {
                session_ = boost::shared_ptr<DAProxy>(new DAProxy(_io_service));

                acceptor_.async_accept(session_->DownstreamSocket(),
                                       boost::bind(&Acceptor::HandleAccept,
                                                   this,
                                                   boost::asio::placeholders::error));
            } catch(std::exception& e) {
                std::cerr << "acceptor exception: " << e.what() << std::endl;
                return false;
            }

            return true;
        }

      private:

        void HandleAccept(const boost::system::error_code& error) {
            if (!error) {
                session_->Start(upstream_host_,upstream_port_);

                if (!AcceptConnections()) {
                    std::cerr << "Failure during call to accept." << std::endl;
                }
            } else {
                std::cerr << "Error: " << error.message() << std::endl;
            }
        }

        boost::asio::io_service& _io_service;
        ip::address_v4 _localhost_address;
        ip::tcp::acceptor acceptor_;
        ptr_type session_;
        unsigned short upstream_port_;
        std::string upstream_host_;
    };

};
}
//TODO: run as a deamon
//daproxy::bridge::Acceptor * pAcceptor;

int StartDAProxy(int argc, char* argv[]) {

    if (argc != 5) {
        std::cerr << "usage: daproxy <local host ip> <local port> <forward host ip> <forward port>" << std::endl;
        return 1;
    }

    const unsigned short local_port   = static_cast<unsigned short>(::atoi(argv[2]));
    const unsigned short forward_port = static_cast<unsigned short>(::atoi(argv[4]));
    const std::string local_host      = argv[1];
    const std::string forward_host    = argv[3];

    //TODO: run as a deamon:
    /*
    std::string run_as_deamon;

    if (argc > 5) {
        run_as_deamon = argv[5];
        if (run_as_deamon == "-d") {
            signal(SIGINT, handleSignal);
            signal(SIGTERM, handleSignal);

            if(RunAsDaemon()) //if parent process
                return EXIT_SUCCESS;
        }
    }
    */

    boost::asio::io_service ios;

    try {
        daproxy::DAProxy::Acceptor acceptor(ios,
                                            local_host, local_port,
                                            forward_host, forward_port);

        acceptor.AcceptConnections();
        ios.run();
    } catch(std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
//TODO: run as a deamon
/*
bool RunAsDaemon() {
    if(fork())
        return true;

    setsid();
    sigignore(SIGHUP);

    if(fork())
        return true;

    umask(0);

    close(0);
    close(1);
    close(2);

    int iDevNull = open("/dev/null", O_RDONLY);
    dup(iDevNull);
    dup(iDevNull);

    return false;
}
void handleSignal(int intSignal) {
    switch (intSignal) {
    case(SIGINT, SIGTERM): {
        //logging;

        //exit(intSignal);
        break;
    }
    }
}
*/
