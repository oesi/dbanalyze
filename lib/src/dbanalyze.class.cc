#include "dbanalyze.h"
#include "database.class.h"
#include "dbanalyze.class.h"
#include "constraint_fk.class.h"
#include "constraint_uk.class.h"
#include "table.class.h"
#include <iostream>
#include <sstream>

dbanalyze::dbanalyze()
{
}

dbanalyze::dbanalyze(std::string type, std::string host, int port, std::string user, std::string password, std::string dbname)
{
	this->connect(type, host, port, user, password, dbname);
}

bool dbanalyze::connect(std::string type, std::string host, int port, std::string user, std::string password, std::string dbname)
{
	this->db = new database();
	return db->connect(type, host, port, user, password, dbname);
}

void dbanalyze::loadData()
{
	this->loadTables();
	this->loadColumns();
	this->loadConstraints();
	this->loadTableSize();
}

void dbanalyze::loadTables()
{
	this->db->loadTables();

	std::string table_namespace, table_name, table_comment;

	while(this->db->nextRow())
	{
		table_namespace = this->db->get("table_schema");
		table_name = this->db->get("table_name");
		table_comment = this->db->get("table_comments");

		this->tablelist.push_back(table(table_namespace, table_name, table_comment));
	}
}

void dbanalyze::loadColumns()
{
	std::string column_name, datatype, character_maximum_length, numeric_precision;
	std::string numeric_scale, is_nullable, table_schema, table_name;
	table* tablepntr;
	std::string last_table_schema, last_table_name;
	this->db->loadColumns();

	while(this->db->nextRow())
	{
		table_schema = this->db->get("table_schema");
		table_name = this->db->get("table_name");
		column_name = this->db->get("column_name");
		datatype = this->db->get("data_type");
		numeric_precision = this->db->get("numeric_precision");
		numeric_scale = this->db->get("numeric_scale");
		is_nullable = this->db->get("is_nullable");
		table_schema = this->db->get("table_schema");
		table_name = this->db->get("table_name");
		character_maximum_length = this->db->get("character_maximum_length");

		// Some Columns are surrounded by double qoutes
		// Remove the qoutes if exists
		if(column_name[0]=='"')
			column_name = column_name.substr(1,column_name.length()-2);

		tablepntr = getTable(table_schema, table_name);
		if(tablepntr!=NULL)
			tablepntr->columnlist.push_back(column(tablepntr, column_name, datatype, character_maximum_length, numeric_precision, numeric_scale, is_nullable));
		else
			std::cerr << "Table not found:" << table_schema << "." << table_name;
	}
}

void dbanalyze::loadConstraints()
{
	this->db->loadConstraints();

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
		fk_table_schema = this->db->get("ref_table_schema");
		fk_table_name = this->db->get("ref_table_name");
		fk_column_name = this->db->get("ref_column_name");

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
			column* src_col = this->getColumn(table_schema, table_name, column_name);
			fk->addSource(src_col);
			column* col = this->getColumn(fk_table_schema, fk_table_name, fk_column_name);
			fk->addTarget(col);
			tablepntr->constraintlist.push_back(fk);
		}
		else if(constraint_type=="UNIQUE")
		{
			uk = new constraint_uk(constraint_schema, constraint_name);
			column* src_col = this->getColumn(table_schema, table_name, column_name);
			uk->addSource(src_col);
			tablepntr->constraintlist.push_back(uk);
		}
	}
}

void dbanalyze::loadTableSize()
{
	for(unsigned int i = 0; i < this->tablelist.size(); i++)
	{
		this->db->getTableSize(this->tablelist[i].schemaname, this->tablelist[i].tablename);

		if(this->db->nextRow())
		{
			this->tablelist[i].row_count = (long)this->db->getNumber("row_count");
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

column* dbanalyze::getColumn(std::string schemaname, std::string tablename, std::string columnname)
{
	table* tbl = this->getTable(schemaname, tablename);
	if(tbl!=NULL)
	{
		for(unsigned int i = 0; i < tbl->columnlist.size(); i++)
		{
			if(tbl->columnlist[i].columnname == columnname)
			{
				return &tbl->columnlist[i];
			}
		}
	}
	return NULL;
}

dbanalyze::~dbanalyze()
{
	delete this->db;
}