#include "dbanalyze.h"
#include "utils.h"
#include "output_cli.class.h"
#include "formatter.h"

#include <vector>
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
		("type,t", po::value<std::string>(), "Type - PostgreSQL, MySQL, SQLite")
		("dbname,d", po::value<std::string>(), "Database Name")
		("graph,g", po::value<std::string>(), "Graph")
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

	if(type!="PostgreSQL" && type!="MySQL" && type!="SQLite" && type!="MSAccess")
	{
		std::cerr << "Unknown type:" << type << std::endl << desc << std::endl;
		return 1;
	}

	if (vm.count("password"))
		password = vm["password"].as<std::string>();
	else if(type!="SQLite" && type!="MSAccess")
	{
		std::cout << "Password:";
		SetStdinEcho(false);
		std::cin >> password;
		SetStdinEcho(true);
		std::cout << std::endl;
	}

	//std::cout << "Analyzing data ..." << std::endl;
	dbanalyze dba(type, host, port, user, password, dbname);
	dba.loadData();

	output_cli out;

	if (vm.count("graph"))
	{
		out.printGraph(dba.getTablelist());
	}
	else
	{
		out.printData(dba.getTablelist());

		statistic stat;
		stat.analyze(dba.getTablelist());
		out.printStatistic(&stat);
	}
	return 0;
}
