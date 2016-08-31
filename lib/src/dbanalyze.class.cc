#include "dbanalyze.h"
#include "database.class.h"
#include "log.h"
#include "dbanalyze.class.h"
#include "constraint_fk.class.h"
#include "constraint_uk.class.h"
#include "table.class.h"
#include <iostream>

dbanalyze::dbanalyze(std::string type, std::string host, int port, std::string user, std::string password, std::string dbname)
{
	this->db = new database(type, host, port, user, password, dbname);
}

bool dbanalyze::loadTables()
{
	this->db->query("SELECT table_schema, table_name FROM information_schema.tables WHERE table_schema NOT IN('pg_catalog','information_schema') AND table_type='BASE TABLE'");

	std::string schemaname, tablename;	

	while(this->db->nextRow())
	{
		schemaname = this->db->get("table_schema");
		tablename = this->db->get("table_name");
		this->tablelist.push_back(table(schemaname,tablename));
	}
}

void dbanalyze::loadColumns()
{
	std::stringstream sql;
	sql << "SELECT column_name, data_type, character_maximum_length,";
	sql << "numeric_precision, numeric_scale,is_nullable, table_schema, table_name ";
	sql << " FROM information_schema.columns WHERE table_schema not in('pg_catalog','information_schema') ";
	sql << " AND EXISTS(SELECT 1 FROM information_schema.tables WHERE table_name=columns.table_name AND table_schema=columns.table_schema AND table_type='BASE TABLE') ";
	sql << " ORDER BY table_schema, table_name";

	this->db->query(sql.str());

	std::string column_name, datatype, character_maximum_length, numeric_precision;
	std::string numeric_scale, is_nullable, table_schema, table_name;
	table* tablepntr;
	std::string last_table_schema, last_table_name;

	while(this->db->nextRow())
	{
		column_name = this->db->get("column_name");
		datatype = this->db->get("data_type");
		character_maximum_length = this->db->get("character_maximum_length");
		numeric_precision = this->db->get("numeric_precision");
		numeric_scale = this->db->get("numeric_scale");
		is_nullable = this->db->get("is_nullable");
		table_schema = this->db->get("table_schema");
		table_name = this->db->get("table_name");

		if(last_table_schema!=table_schema || last_table_name!=table_name)
		{
			tablepntr = this->getTable(table_schema, table_name);
			last_table_schema = table_schema;
			last_table_name = table_name;
		}

		if(tablepntr)
		{
			tablepntr->columnlist.push_back(column(column_name, datatype, character_maximum_length, numeric_precision, numeric_scale, is_nullable));
		}
	}
}

void dbanalyze::loadConstraints()
{
	std::stringstream sql;

	sql << "SELECT table_constraints.constraint_type, table_constraints.constraint_schema, table_constraints.constraint_name, constraint_column_usage.table_schema, ";
	sql << " constraint_column_usage.table_name, constraint_column_usage.column_name, ";
	sql << " ccu.table_schema as fk_table_schema, ccu.table_name as fk_table_name, ccu.column_name as fk_column_name ";
	sql << "FROM information_schema.table_constraints ";
	sql << "JOIN information_schema.constraint_column_usage USING(constraint_name, constraint_schema) ";
	sql << "LEFT JOIN information_schema.referential_constraints USING(constraint_name, constraint_schema) ";
	sql << "LEFT JOIN information_schema.constraint_column_usage ccu on(ccu.constraint_name=unique_constraint_name AND ccu.constraint_schema=unique_constraint_schema) ";
	sql << "ORDER BY fk_table_schema, fk_table_name, table_schema, table_name";

	this->db->query(sql.str());

	std::string constraint_type, constraint_schema, constraint_name, table_schema;
	std::string table_name, column_name, fk_table_schema, fk_table_name, fk_column_name;
	constraint_fk *fk=NULL;
	constraint_uk *uk=NULL;
	table* tablepntr;

	while(this->db->nextRow())
	{
		constraint_type = this->db->get("constraint_type");
		constraint_schema = this->db->get("constraint_schema");
		constraint_name = this->db->get("constraint_name");
		table_schema = this->db->get("table_schema");
		table_name = this->db->get("table_name");
		column_name = this->db->get("column_name");
		fk_table_schema = this->db->get("fk_table_schema");;
		fk_table_name = this->db->get("fk_table_name");
		fk_column_name = this->db->get("fk_column_name");

		tablepntr = NULL;
		tablepntr = this->getTable(table_schema, table_name);
		if(tablepntr==NULL)
			continue;

		if(constraint_type=="PRIMARY KEY")
		{
			tablepntr->pk.push_back(column_name);
		}
		else if(constraint_type=="FOREIGN KEY")
		{
			fk = new constraint_fk(constraint_schema, constraint_name);
			fk->addSource(table_schema, table_name, column_name);
			fk->addTarget(fk_table_schema, fk_table_name, fk_column_name);
			tablepntr->constraintlist.push_back(fk);

		}
		else if(constraint_type=="UNIQUE")
		{
			uk = new constraint_uk(constraint_schema, constraint_name);
			uk->addSource(table_schema, table_name, column_name);
			tablepntr->constraintlist.push_back(uk);
		}

	}

}

table* dbanalyze::getTable(std::string schemaname, std::string tablename)
{
	for(unsigned int i = 0; i < this->tablelist.size(); i++)
	{
		if(this->tablelist[i].schemaname == schemaname && this->tablelist[i].tablename==tablename)
		{
			return &this->tablelist[i];
		}
	}
	return NULL;
}

void dbanalyze::output()
{
	unsigned int i;
	std::cout << std::endl << "== Tables ==" << std::endl;
	for(i = 0; i < this->tablelist.size(); i++)
	{
		tablelist[i].output();
	}
	std::cout << "(" << i << " rows)" << std::endl;
}

dbanalyze::~dbanalyze()
{
	delete this->db;
}