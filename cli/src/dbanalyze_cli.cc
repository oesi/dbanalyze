#include "dbanalyze.h"
#include "utils.h"
#include "output_cli.class.h"

#include <iostream>
#include <boost/program_options.hpp>


namespace po = boost::program_options;

int main (int argc, char *argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("port,p", po::value<int>(), "Port")
		("host,h", po::value<std::string>(), "Host")
		("user,u", po::value<std::string>(), "User")
		("password,w", po::value<std::string>(), "Password")
		("type,t", po::value<std::string>(), "Type - PostgreSQL")
		("dbname,d", po::value<std::string>(), "Database Name")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << desc << "\n";
		return 1;
	}
	int port;
	std::string host, user, dbname, type, password;

	if (vm.count("port"))
		port = vm["port"].as<int>();

	if (vm.count("host"))
		host = vm["host"].as<std::string>();

	if (vm.count("user"))
		user = vm["user"].as<std::string>();

	if (vm.count("dbname"))
		dbname = vm["dbname"].as<std::string>();

	if (vm.count("type"))
		type = vm["type"].as<std::string>();

	if (vm.count("password"))
		password = vm["password"].as<std::string>();
	else
	{
		std::cout << "Password:";
		SetStdinEcho(false);
		std::cin >> password;
		SetStdinEcho(true);
	}

	dbanalyze dba(type, host, port, user, password, dbname);
	dba.loadData();

	output_cli out;
	out.printData(dba.getTablelist());

	statistic stat;
	stat.analyze(dba.getTablelist());
	stat.output();

	return 0;
}
