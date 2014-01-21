/*
 Steve Siska
 login: siska
 CS564
 Project 4
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include "sqlite3.h"

using namespace std;

//forward declarations
void createTable(string table_name, sqlite3 *db);
void insert_tuples(sqlite3 *db);
void parseAndInsert(string file_name, string table_name, sqlite3 *db);
vector<string>* parse_line(string line);
string commaSeperateAttributes(vector<string>* attrs);


/**
 Create a log file that records what happens
 */
int callback(void* p, int argc, char **argv, char **column){
    ofstream logfile;
    logfile.open("log.txt");
    for(int i = 0; i < argc; i++){
        cerr << argv[i] << endl;
        logfile << argv[i];
    }
    logfile.close();
    return 0;
}

/**
    Creates a table in sqlite3 using the specified table name
 */
void createTable(string table_name, sqlite3 *db){
    char* error;
    sqlite3_exec(db, table_name.c_str(), callback, 0, &error);
    if(error != NULL){
        cerr << error << endl;
    }
    sqlite3_free(error);
}
/**
 Drops a table in sqlite3 using the specified table name
 */
void dropTable(string table_name, sqlite3 *db){
    char* error;
    string dt = "DROP TABLE IF EXISTS " + table_name + ";";
    sqlite3_exec(db, dt.c_str(), callback, 0, &error);
    cerr << error << endl;
}

/**
    Inserts tuples to the database for each given file
 */
void insert_tuples(sqlite3* db){
    parseAndInsert("Division.txt", "division_codes", db);
    parseAndInsert("Origin.txt", "origin_codes", db);
    parseAndInsert("Race.txt", "race_codes", db);
    parseAndInsert("Region.txt", "region_codes", db);
    parseAndInsert("Sex.txt", "sex_codes", db);
    parseAndInsert("Sumlev.txt", "sumlev_codes", db);
    parseAndInsert("Pop_Estimate_Nation_state_PR.txt", "Pop_Est_Nation_State_PR", db);
    parseAndInsert("Pop_Estimate_State_County.txt", "Pop_Est_State_County_PR", db);
    parseAndInsert("Pop_Estimate_Cities_Towns.txt", "pop_est_cities_towns", db);
    parseAndInsert("Pop_Estimate_State_Age_Sex_Race_Origin.txt", "pop_state_age_sex_race_origin", db);
    parseAndInsert("Pop_Estimate_PR_Sex_Age.txt", "pop_est_pr_sex_age", db);
    parseAndInsert("Pop_Estimate_PR_Municipios.txt", "pop_est_pr_municipios", db);
    parseAndInsert("Pop_Estimate_PR_Mun_Sex_Age.txt", "pop_est_pr_mun_sex_age", db);
    parseAndInsert("Housing_Units_State_Level.txt", "hu_state_level", db);
    parseAndInsert("Pop_Estimate_Metro_Micro.txt", "pop_est_metro_micro", db);
}

/**
    Parses a line into a vector of attributes, to be inserted in a SQL table
 */
vector<string>* parse_line(string line){
    vector<string>* line_attrs = new vector<string>();
    bool quoteFound = false;
    string* newString = new string();
    newString->push_back('\'');
    for(int i = 0; i < line.length(); i++){
        char& c = line.at(i);
        if(c == '"'){
            quoteFound = !quoteFound;
        }
        else if((c == ',' && !quoteFound) || c == 13){
            if(newString->empty()){
                newString->operator+=("NULL");
            }
            line_attrs->push_back(*newString+="'");
            delete newString;
            newString = new string();
            newString->push_back('\'');
        }
        else if(c == '\''){
            newString->push_back('\'');
            newString->push_back(c);
        }
        else{
            newString->push_back(c);
        }
    }
    if(newString->at(newString->size()-1) != '\''){
        newString->push_back('\'');
        line_attrs->push_back(*newString);
    }
    return line_attrs;
}

/**
 Takes a vector of strings and converts it into a string of comma seperated tokens
 */
string commaSeperateAttributes(vector<string>* attrs){
    stringstream s;
    const string delim = ",";
    copy(attrs->begin(), attrs->end(), ostream_iterator<string>(s, delim.c_str()));
    string out = s.str();
    out = out.substr(0, out.size()-1);
    return out;
}

/**
    Parses each line of the file and inserts that line into the appropriate SQL table
 */
void parseAndInsert(string file_name, string table_name, sqlite3 *db){
    cerr << "Inserting in to " + table_name + "\n";
    vector<string>* attributes;
    ifstream infile;
    infile.open(file_name.c_str());
    if(!infile.is_open()){
        cerr << "Couldn't open file " + file_name+  "\n" << endl;
    }
    string line = "";
    char * errmsg;
    getline(infile, line); //discard first line, as it has column names
    sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &errmsg);
    while(getline(infile, line)){
        //get a line of attributes to insert
        attributes = parse_line(line);
        //insert into correct table
        string insert_string = "INSERT INTO " + table_name + " VALUES (" + commaSeperateAttributes(attributes) + ");";
        
        sqlite3_exec(db, insert_string.c_str(), NULL, NULL, &errmsg);
        if(errmsg != NULL){
            cerr << errmsg << endl;
        }
        sqlite3_free(errmsg);
    }
    sqlite3_exec(db, "COMMIT TRANSACTION", NULL, NULL, &errmsg);
}



int main(int argc, const char * argv[])
{
    //remove old log file
    remove("log.txt");
    //remove database file
    remove("census.db");
    //open database connection
    int ret;
    sqlite3 *db;
    ret = sqlite3_open("census.db", &db);
    if(ret){
        cerr << "Error in opening the database!\n" << endl;
        exit(-1);
    }
    
    //create division_codes table
    string create_division_codes =
        "CREATE TABLE IF NOT EXISTS division_codes ("
        "Division_Cd INTEGER,"
        "Division_Desc TEXT"
        ");";
    //dropTable("create_division_codes", db);
    createTable(create_division_codes, db);
    
    //create origin_codes table
    string create_origin_codes =
        "CREATE TABLE IF NOT EXISTS origin_codes ("
        "Origin_Cd INTEGER,"
        "Origin_Desc TEXT"
        ");";
    
    //dropTable("create_origin_codes", db);
    createTable(create_origin_codes, db);
    
    //create race_codes table
    string create_race_codes =
        "CREATE TABLE IF NOT EXISTS race_codes ("
        "Race_Cd INTEGER,"
        "Race_Desc TEXT"
        ");";
    createTable(create_race_codes, db);
    
    //create region_codes table
    string create_region_codes =
        "CREATE TABLE IF NOT EXISTS region_codes ("
        "Region_Cd INTEGER,"
        "Region_Desc TEXT"
        ");";
    createTable(create_region_codes, db);
    
    //create sex_codes table
    string create_sex_codes =
        "CREATE TABLE IF NOT EXISTS sex_codes ("
        "Sex_Cd INTEGER,"
        "Sex_Desc TEXT"
        ");";
    createTable(create_sex_codes, db);
    
    //create sumlev_codes table
    string create_sumlev_codes =
        "CREATE TABLE IF NOT EXISTS sumlev_codes ("
        "Sumlev_Cd INTEGER,"
        "Sumlev_Desc TEXT"
        ");";
    createTable(create_sumlev_codes, db);
    
    //create POP_EST_NATION_STATE_PR table
    string create_pop_est_nation_state_pr =
        "CREATE TABLE IF NOT EXISTS Pop_Est_Nation_State_PR ("
        "sumlev INTEGER,"
        "region INTEGER,"
        "division INTEGER,"
        "state INTEGER,"
        "name TEXT,"
        "census2010pop INTEGER,"
        "estimatesbase2010 INTEGER,"
        "popestimate2010 INTEGER,"
        "popestimate2011 INTEGER,"
        "popestimate2012 INTEGER,"
        "npopchg_2010 INTEGER,"
        "npopchg_2011 INTEGER,"
        "npopchg_2012 INTEGER,"
        "births2010 INTEGER,"
        "births2011 INTEGER,"
        "births2012 INTEGER,"
        "deaths2010 INTEGER,"
        "deaths2011 INTEGER,"
        "deaths2012 INTEGER,"
        "naturalinc2010 INTEGER,"
        "naturalinc2011 INTEGER,"
        "naturalinc2012 INTEGER,"
        "internationalmig2010 INTEGER,"
        "internationalmig2011 INTEGER,"
        "internationalmig2012 INTEGER,"
        "domesticmig2010 INTEGER,"
        "domesticmig2011 INTEGER,"
        "domesticmig2012 INTEGER,"
        "netmig2010 INTEGER,"
        "netmig2011 INTEGER,"
        "netmig2012 INTEGER,"
        "residual2010 INTEGER,"
        "residual2011 INTEGER,"
        "residual2012 INTEGER,"
        "rbirth2011 REAL,"
        "rbirth2012 REAL,"
        "rdeath2011 REAL,"
        "rdeath2012 REAL,"
        "rnaturalinc2011 REAL,"
        "rnaturalinc2012 REAL,"
        "rinternationalmig2011 REAL,"
        "rinternationalmig2012 REAL,"
        "rdomesticmig2011 REAL,"
        "rdomesticmig2012 REAL,"
        "rnetmig2011 REAL,"
        "rnetmig2012 REAL,"
        "PRIMARY KEY (name),"
        "FOREIGN KEY (sumlev) REFERENCES sumlev_codes(Sumlev_Cd),"
        "FOREIGN KEY (region) REFERENCES region_codes(Region_Cd),"
        "FOREIGN KEY (division) REFERENCES division_codes(Division_Cd)"
        ");";
    createTable(create_pop_est_nation_state_pr, db);
    
    //create POP_EST_STATE_COUNTY table
    string create_pop_est_state_county =
        "CREATE TABLE IF NOT EXISTS Pop_Est_State_County_PR ("
        "sumlev INTEGER,"
        "region INTEGER,"
        "division INTEGER,"
        "state INTEGER,"
        "county INTEGER,"
        "stname TEXT,"
        "ctyname TEXT,"
        "census2010pop INTEGER,"
        "estimatesbase2010 INTEGER,"
        "popestimate2010 INTEGER,"
        "popestimate2011 INTEGER,"
        "popestimate2012 INTEGER,"
        "npopchg_2010 INTEGER,"
        "npopchg_2011 INTEGER,"
        "npopchg_2012 INTEGER,"
        "births2010 INTEGER,"
        "births2011 INTEGER,"
        "births2012 INTEGER,"
        "deaths2010 INTEGER,"
        "deaths2011 INTEGER,"
        "deaths2012 INTEGER,"
        "naturalinc2010 INTEGER,"
        "naturalinc2011 INTEGER,"
        "naturalinc2012 INTEGER,"
        "internationalmig2010 INTEGER,"
        "internationalmig2011 INTEGER,"
        "internationalmig2012 INTEGER,"
        "domesticmig2010 INTEGER,"
        "domesticmig2011 INTEGER,"
        "domesticmig2012 INTEGER,"
        "netmig2010 INTEGER,"
        "netmig2011 INTEGER,"
        "netmig2012 INTEGER,"
        "residual2010 INTEGER,"
        "residual2011 INTEGER,"
        "residual2012 INTEGER,"
        "gqestimatesbase2010 INTEGER,"
        "gqestimates2010 INTEGER,"
        "gqestimates2011 INTEGER,"
        "gqestimates2012 INTEGER,"
        "rbirth2011 REAL,"
        "rbirth2012 REAL,"
        "rdeath2011 REAL,"
        "rdeath2012 REAL,"
        "rnaturalinc2011 REAL,"
        "rnaturalinc2012 REAL,"
        "rinternationalmig2011 REAL,"
        "rinternationalmig2012 REAL,"
        "rdomesticmig2011 REAL,"
        "rdomesticmig2012 REAL,"
        "rnetmig2011 REAL,"
        "rnetmig2012 REAL,"
        "PRIMARY KEY (state, county),"
        "FOREIGN KEY (sumlev) REFERENCES sumlev_codes(Sumlev_Cd),"
        "FOREIGN KEY (region) REFERENCES region_codes(Region_Cd),"
        "FOREIGN KEY (division) REFERENCES division_codes(Division_Cd)"
        ");";
    createTable(create_pop_est_state_county, db);
    
    //create POP_EST_CITIES_TOWNS table
    string create_pop_est_cities_towns =
        "CREATE TABLE IF NOT EXISTS pop_est_cities_towns ("
        "sumlev INTEGER,"
        "state INTEGER,"
        "county INTEGER,"
        "place INTEGER,"
        "cousub INTEGER,"
        "concit INTEGER,"
        "name TEXT,"
        "stname TEXT,"
        "census2010pop INTEGER,"
        "estimatesbase2010 INTEGER,"
        "popestimate2010 INTEGER,"
        "popestimate2011 INTEGER,"
        "PRIMARY KEY (state, county, place, cousub, concit),"
        "FOREIGN KEY (sumlev) REFERENCES sumlev_codes(Sumlev_Cd)"
        ");";
    createTable(create_pop_est_cities_towns, db);
    
    //create POP_EST_STATE_AGE_SEX_RACE_ORIGIN table
    string create_pop_state_age_sex_race_origin =
        "CREATE TABLE IF NOT EXISTS pop_state_age_sex_race_origin ("
        "sumlev INTEGER,"
        "region INTEGER,"
        "division INTEGER,"
        "state INTEGER,"
        "sex INTEGER,"
        "origin INTEGER,"
        "race INTEGER,"
        "age INTEGER,"
        "census2010pop INTEGER,"
        "estimatesbase2010 INTEGER,"
        "popestimate2010 INTEGER,"
        "popestimate2011 INTEGER,"
        "PRIMARY KEY (state, sex, origin, race, age),"
        "FOREIGN KEY (sumlev) REFERENCES sumlev_codes(Sumlev_Cd),"
        "FOREIGN KEY (region) REFERENCES region_codes(Region_Cd),"
        "FOREIGN KEY (division) REFERENCES division_codes(Division_Cd),"
        "FOREIGN KEY (sex) REFERENCES sex_codes(Sex_Cd),"
        "FOREIGN KEY (origin) REFERENCES origin_codes(Origin_Cd),"
        "FOREIGN KEY (race) REFERENCES race_codes(Race_Cd)"
        ");";
    createTable(create_pop_state_age_sex_race_origin, db);
    
    //create POP_EST_PR_SEX_AGE table
    string create_pop_est_pr_sex_age =
        "CREATE TABLE IF NOT EXISTS pop_est_pr_sex_age ("
        "sumlev INTEGER,"
        "state INTEGER,"
        "name TEXT,"
        "sex INTEGER,"
        "age INTEGER,"
        "census2010pop INTEGER,"
        "estimatesbase2010 INTEGER,"
        "popestimate2010 INTEGER,"
        "popestimate2011 INTEGER,"
        "PRIMARY KEY (sex, age),"
        "FOREIGN KEY (sumlev) REFERENCES sumlev_codes(Sumlev_Cd),"
        "FOREIGN KEY (sex) REFERENCES sex_codes(Sex_Cd)"
        ");";
    createTable(create_pop_est_pr_sex_age, db);
    
    //create POP_EST_PR_MUNICIPIOS table
    string create_pop_est_pr_municipios =
        "CREATE TABLE IF NOT EXISTS pop_est_pr_municipios ("
        "sumlev INTEGER,"
        "municipio INTEGER,"
        "name TEXT,"
        "estimatesbase2010 INTEGER,"
        "popestimate2010 INTEGER,"
        "popestimate2011 INTEGER,"
        "npopchg_2010 REAL,"
        "npopchg_2011 REAL,"
        "ppopchg_2010 REAL,"
        "ppopchg_2011 REAL,"
        "srank_estbase2010 INTEGER,"
        "srank_popest2010 INTEGER,"
        "srank_popest2011 INTEGER,"
        "srank_npchg2010 INTEGER,"
        "srank_npchg2011 INTEGER,"
        "srank_ppchg2010 INTEGER,"
        "srank_ppchg2011 INTEGER,"
        "PRIMARY KEY (municipio),"
        "FOREIGN KEY (sumlev) REFERENCES sumlev_codes(Sumlev_Cd)"
        ");";
    createTable(create_pop_est_pr_municipios, db);
    
    //create POP_EST_PR_MUN_SEX_AGE table
    string create_pop_est_pr_mun_sex_age =
        "CREATE TABLE IF NOT EXISTS pop_est_pr_mun_sex_age ("
        "sumlev INTEGER,"
        "municipio INTEGER,"
        "name TEXT,"
        "year INTEGER,"
        "popestimate INTEGER,"
        "popest_male INTEGER,"
        "popest_fem INTEGER,"
        "under5_tot INTEGER,"
        "under5_male INTEGER,"
        "under5_fem INTEGER,"
        "age513_tot INTEGER,"
        "age513_male INTEGER,"
        "age513_fem INTEGER,"
        "age1417_tot INTEGER,"
        "age1417_male INTEGER,"
        "age1417_fem INTEGER,"
        "age1824_tot INTEGER,"
        "age1824_male INTEGER,"
        "age1824_fem INTEGER,"
        "age16plus_tot INTEGER,"
        "age16plus_male INTEGER,"
        "age16plus_fem INTEGER,"
        "age18plus_tot INTEGER,"
        "age18plus_male INTEGER,"
        "age18plus_fem INTEGER,"
        "age1544_tot INTEGER,"
        "age1544_male INTEGER,"
        "age1544_fem INTEGER,"
        "age2544_tot INTEGER,"
        "age2544_male INTEGER,"
        "age2544_fem INTEGER,"
        "age4564_tot INTEGER,"
        "age4564_male INTEGER,"
        "age4564_fem INTEGER,"
        "age65plus_tot INTEGER,"
        "age65plus_male INTEGER,"
        "age65plus_fem INTEGER,"
        "age85plus_tot INTEGER,"
        "age85plus_male INTEGER,"
        "age85plus_fem INTEGER,"
        "median_age_tot INTEGER,"
        "median_age_male INTEGER,"
        "median_age_fem INTEGER,"
        "PRIMARY KEY (municipio, year),"
        "FOREIGN KEY (sumlev) REFERENCES sumlev_codes(Sumlev_Cd)"
        ");";
    createTable(create_pop_est_pr_mun_sex_age, db);
  
    //create POP_EST_METRO_MICRO table
    string create_pop_est_metro_micro =
        "CREATE TABLE IF NOT EXISTS pop_est_metro_micro ("
        "csba INTEGER,"
        "mdiv INTEGER,"
        "stcou INTEGER,"
        "name TEXT,"
        "lsad TEXT,"
        "census2010pop INTEGER,"
        "estimatesbase2010 INTEGER,"
        "popestimate2010 INTEGER,"
        "popestimate2011 INTEGER,"
        "npopchg_2010 INTEGER,"
        "npopchg_2011 INTEGER,"
        "naturalinc2010 INTEGER,"
        "naturalinc2011 INTEGER,"
        "births2010 INTEGER,"
        "births2011 INTEGER,"
        "deaths2010 INTEGER,"
        "deaths2011 INTEGER,"
        "netmig2010 INTEGER,"
        "netmig2011 INTEGER,"
        "internationalmig2010 INTEGER,"
        "internationalmig2011 INTEGER,"
        "domesticmig2010 INTEGER,"
        "domesticmig2011 INTEGER,"
        "residual2010 INTEGER,"
        "residual2011 INTEGER,"
        "PRIMARY KEY (name, lsad)"
        ");";
    createTable(create_pop_est_metro_micro, db);
    
    //create HU_UNIT_STATE_LEVEL table
    string create_hu_state_level =
        "CREATE TABLE IF NOT EXISTS hu_state_level ("
        "sumlev INTEGER,"
        "state INTEGER,"
        "region INTEGER,"
        "division INTEGER,"
        "stname TEXT,"
        "hucensus2010 INTEGER,"
        "huestbase2010 INTEGER,"
        "huest_2010 INTEGER,"
        "huest_2011 INTEGER,"
        "PRIMARY KEY (state),"
        "FOREIGN KEY (sumlev) REFERENCES sumlev_codes(Sumlev_Cd),"
        "FOREIGN KEY (region) REFERENCES region_codes(Region_Cd),"
        "FOREIGN KEY (division) REFERENCES division_codes(Division_Cd)"
        ");";
    createTable(create_hu_state_level, db);
    
    insert_tuples(db);
    
    
    return 0;
}

