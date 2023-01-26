//  C0F4DFE8B340D81183C208F70F9D2D797908754D
//  main.cpp
//  p3
//
//  Created by Lucas Sun on 11/1/22.
//

#include <iostream>
#include <getopt.h>
#include <iostream>
#include <istream>
#include <map>
#include <unordered_map>
#include <vector>
#include <deque>
#include <algorithm>
#include <iostream>
#include <queue>
#include "TableEntry.h"
#include <cstdlib>
#include <utility>

#include "xcode_redirect.hpp"

using namespace std;

class Equals{
public:
    TableEntry entry;
    size_t index;
    Equals(TableEntry temp_entry, size_t temp_index): entry(temp_entry),index(temp_index){}
    bool operator()(const vector<TableEntry> &m)const{
        if(entry==m[index])
            return true;
        else
            return false;
    }
};
class Greater{
public:
    TableEntry entry;
    size_t index;
    Greater(TableEntry temp_entry, size_t temp_index): entry(temp_entry),index(temp_index){}
    bool operator()(const vector<TableEntry> &m)const{
        if(entry<m[index])
            return true;
        else
            return false;
    }
};
class Less{
public:
    TableEntry entry;
    size_t index;
    Less(TableEntry temp_entry, size_t temp_index): entry(temp_entry),index(temp_index){}
    bool operator()( const vector<TableEntry> &m)const{
        if(entry>m[index])
            return true;
       else
           return false;
    }
};
class Table{
public:
    string name;
    vector<vector<TableEntry>> table;
    vector<EntryType> column_type;
    vector<string> column_name;
    string conditional_column;
    unordered_map<TableEntry, vector<size_t>> hashkey;
    char type;
    map<TableEntry, vector<size_t>> bstkey;
    Table(){};
    Table(string temp) : name(temp){};
};


class Program{
private:
    unordered_map<string, Table> hashmap;
public:
    bool quiet = false;
    void run();
    void get_mode(int argc, char * argv[]);
    void run_create();
    void run_remove();
    void run_insert();
    void run_print();
    void run_delete();
    void run_generate();
    void run_join();
    size_t find_index(string, string);
    TableEntry four_helper(string, size_t);
    bool three_helper(string, char, TableEntry, size_t);
};



void Program::run(){
    string cmd;
    do{
        cout<<"% ";
        cin>>cmd;
        switch(cmd[0]){
            case'#':
                getline(cin,cmd);
                break;
            case'C':
                run_create();
                break;
            case'R':
                run_remove();
                break;
            case'D':
                run_delete();
                break;
            case'I':
                run_insert();
                break;
            case'P':
                run_print();
                break;
            case'G':
                run_generate();
                break;
            case'J':
                run_join();
                break;
            case'Q':
                break;
            default:
                cout<<"Error: unrecognized command\n";
                getline(cin,cmd);
                break;
        }
    }while(cmd!="QUIT");
    cout<<"Thanks for being silly!";
}

void Program::get_mode(int argc, char * argv[]){
    int option_index = 0, option =0;
    struct option long_options[] = {
        {"help"     ,   no_argument,       nullptr, 'h'},
        {"quiet"   ,   no_argument,       nullptr, 'q'},
        { nullptr, 0,                      nullptr, '\0' }
    };
    while ((option = getopt_long(argc, argv, "hq", long_options, &option_index)) != -1) {
        switch (option) {
            case 'h':
                cout<<"This is a help message";
                exit(0);
                break;
            case 'q':
                quiet = true;
                break;
        }
    }
}

void Program::run_create(){
    string name;
    int col;
    string input;
    cin>>name;
    if(hashmap.find(name)!=hashmap.end()){
        cout<<"Error during CREATE: Cannot create already existing table "<<name<<'\n';
        getline(cin,input);
        return;
    }
    Table temp{name};
    cin>>col;
    for(int i=0; i< col; ++i){
        cin>>input;
        if(input=="string"){
            temp.column_type.push_back(EntryType::String);
        }
        else if(input=="bool"){
            temp.column_type.push_back(EntryType::Bool);
        }
        else if(input=="int"){
            temp.column_type.push_back(EntryType::Int);
        }
        else if(input=="double"){
            temp.column_type.push_back(EntryType::Double);
        }
    }
    cout <<"New table "<<name<<" with column(s) ";
    for(int i=0; i< col; ++i){
        cin>>input;
        cout<<input<<" ";
        temp.column_name.push_back(input);

    }
    hashmap.insert(make_pair(name, temp));
    cout<<"created\n";
}
void Program::run_remove(){
    string input;
    cin>>input;
    if(hashmap.find(input)==hashmap.end()){
        cout<<"Error during REMOVE: "<<input<<" does not name a table in the database\n";
        getline(cin,input);
        return;
    }
    cout<<"Table "<<input<<" deleted\n";
    hashmap.erase(input);
}

void Program::run_insert(){
    size_t entries;
    string filename;
    string c;
    cin>>c;
    cin>>filename;
    cin>>entries;
    cin>>c;
    if(hashmap.find(filename)==hashmap.end()){
        cout<<"Error during INSERT: "<<filename<<" does not name a table in the database\n";
        getline(cin,c);
        return;
    }
    cout<<"Added "<<entries<<" rows to "<<filename<<" from position "<<hashmap[filename].table.size()<<" to "<<hashmap[filename].table.size()+ entries-1<<'\n';
    size_t size =hashmap[filename].column_type.size();
    size_t start =hashmap[filename].table.size();
    hashmap[filename].table.reserve(size+entries);
    size_t index = find_index(filename, hashmap[filename].conditional_column);
    
    for(size_t i = 0; i < entries; ++i){
        vector<TableEntry> row;
        row.reserve(hashmap[filename].column_type.size());
        for(uint32_t j = 0; j < hashmap[filename].column_type.size(); ++j){
            switch(hashmap[filename].column_type[j]){
                case EntryType::String:
                {
                    string s_input;
                    cin>>s_input;
                    row.emplace_back(TableEntry(s_input));
                    break;

                }
                case EntryType::Double:
                {
                    double d_input;
                    cin>>d_input;
                    row.emplace_back(TableEntry(d_input));
                    break;
                }
                case EntryType::Int:
                {
                    int i_input;
                    cin>>i_input;
                    row.emplace_back(TableEntry(i_input));
                    break;
                }
                case EntryType::Bool:
                {
                    bool b_input;
                    cin>>b_input;
                    row.emplace_back(TableEntry(b_input));
                    break;
                }
            }
        }
        hashmap[filename].table.push_back(row);
        if(hashmap[filename].type=='h'){
            TableEntry key =row[index];
            hashmap[filename].hashkey[key].push_back(start+i);
        }
        if(hashmap[filename].type=='b'){
            TableEntry key =row[index];
            hashmap[filename].bstkey[key].push_back(start+i);
        }
    }
}

TableEntry Program::four_helper(string filename, size_t index){
    switch(hashmap[filename].column_type[index]){
        case EntryType::String:
        {
            string condition;
            cin>>condition;
            TableEntry entry = TableEntry(condition);
            return entry;

        }
        case EntryType::Double:
        {
            double condition;
            cin>>condition;
            TableEntry entry = TableEntry(condition);
            return entry;
        }
        case EntryType::Int:
        {
            int condition;
            cin>>condition;
            TableEntry entry = TableEntry(condition);
            return entry;
        }
        case EntryType::Bool:
        {
            bool condition;
            cin>>condition;
            TableEntry entry = TableEntry(condition);
            return entry;
        }
    }
    string condition;
    cin>>condition;
    TableEntry entry = TableEntry(condition);
    return(entry);
}

void Program::run_print(){
    string mode;
    string filename;
    int entries;
    string column_name;
    size_t row_num = 0;
    cin>>mode>>filename>>entries;
    if(hashmap.find(filename)==hashmap.end()){
        cout<<"Error during PRINT: "<<filename<<" does not name a table in the database\n";
        getline(cin,filename);
        return;
    }
    vector<size_t> indices;
    
    if(quiet){
        for(int j =0; j<entries; ++j){
            cin>>column_name;
            size_t index = find_index(filename, column_name);
            if(index==hashmap[filename].column_name.size()){
                cout<<"Error during PRINT: "<<column_name<<" does not name a column in "<<filename<<'\n';
                getline(cin,filename);
                return;
            }
            indices.push_back(index);
        }
    }
    else{
        for(int j =0; j<entries; ++j){
            cin>>column_name;
            size_t index = find_index(filename, column_name);
            if(index==hashmap[filename].column_name.size()){
                cout<<"Error during PRINT: "<<column_name<<" does not name a column in "<<filename<<'\n';
                getline(cin,filename);
                return;
            }
            indices.push_back(index);
        }
    }
    cin >> mode;
    if(mode[0]=='A'){
        if(!quiet){
            for(size_t i = 0; i<indices.size();++i){
                cout<<hashmap[filename].column_name[indices[i]]<<" ";
            }
            cout<<'\n';
        }
        if(quiet){
            for(size_t i =0; i < hashmap[filename].table.size(); ++i){
                ++row_num;
            }
        }
        else{
            for(size_t i =0; i < hashmap[filename].table.size(); ++i){
                for(size_t j =0; j<indices.size(); ++j){
                    cout << hashmap[filename].table[i][indices[j]] <<" ";
                }
                ++row_num;
                cout<<'\n';
            }
        }
    }
    else{
        string column_name;
        char op;
        cin>>column_name>>op;
        size_t index = find_index(filename, column_name);
        if(index==hashmap[filename].column_name.size()){
            cout<<"Error during PRINT: "<<column_name<<" does not name a column in "<<filename<<'\n';
            getline(cin,filename);
            return;
        }
        if(!quiet){
            for(size_t i = 0; i<indices.size();++i){
                cout<<hashmap[filename].column_name[indices[i]]<<" ";
            }
            cout<<'\n';
        }
        TableEntry entry = four_helper(filename, index);
        if(quiet){
            switch(op){
                case '=':
                {
                    if(!hashmap[filename].hashkey.empty() && column_name==hashmap[filename].conditional_column){
                        for(size_t i =0; i < hashmap[filename].hashkey[entry].size(); ++i){
                            ++row_num;
                        }
                    }
                    else{
                        for(size_t i = 0 ; i <hashmap[filename].table.size();++i){
                            if(hashmap[filename].table[i][index] == entry){
                                ++row_num;
                            }
                        }
                    }
                    break;
                }
                case '>':
                {
                    if(!hashmap[filename].bstkey.empty() && hashmap[filename].conditional_column==column_name){
                        auto low = hashmap[filename].bstkey.upper_bound(entry);
                        for(auto it=low;it!=hashmap[filename].bstkey.end();++it){
                            for(size_t i = 0; i<it->second.size(); ++i){
                                ++row_num;
                            }
                        }
                    }
                    else{
                        for(size_t i = 0 ; i <hashmap[filename].table.size();++i){
                            if(hashmap[filename].table[i][index] > entry){
                                ++row_num;
                            }
                        }
                    }
                    break;
                }
                case '<':
                {
                    if(!hashmap[filename].bstkey.empty() && hashmap[filename].conditional_column==column_name){
                        auto high = hashmap[filename].bstkey.lower_bound(entry);
                        for(auto it=hashmap[filename].bstkey.begin();it!=high;++it){
                            for(size_t i = 0; i<it->second.size(); ++i){
                                ++row_num;
                            }
                        }
                    }
                    else{
                        for(size_t i = 0 ; i <hashmap[filename].table.size();++i){
                            if(hashmap[filename].table[i][index] < entry){
                                ++row_num;
                            }
                        }
                    }
                    break;
                }
            }
        }
        else{
            switch(op){
                case '=':
                {
                    if(!hashmap[filename].hashkey.empty() && column_name==hashmap[filename].conditional_column){
                        for(size_t i =0; i < hashmap[filename].hashkey[entry].size(); ++i){
                            for(size_t j =0; j<indices.size(); ++j){
                                cout << hashmap[filename].table[hashmap[filename].hashkey[entry][i]][indices[j]] <<" ";
                            }
                            ++row_num;
                            cout<<'\n';
                        }
                    }
                    else{
                        for(size_t i = 0 ; i <hashmap[filename].table.size();++i){
                            if(hashmap[filename].table[i][index] == entry){
                                for(size_t j =0; j<indices.size(); ++j){
                                    cout << hashmap[filename].table[i][indices[j]] <<" ";
                                }
                                ++row_num;
                                cout<<'\n';
                            }
                        }
                    }
                    break;
                }
                case '>':
                {
                    if(!hashmap[filename].bstkey.empty() && hashmap[filename].conditional_column==column_name){
                        auto low = hashmap[filename].bstkey.upper_bound(entry);
                        for(auto it=low;it!=hashmap[filename].bstkey.end();++it){
                            for(size_t i = 0; i<it->second.size(); ++i){
                                for(size_t j =0; j<indices.size(); ++j){
                                    cout << hashmap[filename].table[it->second[i]][indices[j]] <<" ";
                                }
                                ++row_num;
                                cout<<'\n';
                            }
                        }
                    }
                    else{
                        for(size_t i = 0 ; i <hashmap[filename].table.size();++i){
                            if(hashmap[filename].table[i][index] > entry){
                                for(size_t j =0; j<indices.size(); ++j){
                                    cout << hashmap[filename].table[i][indices[j]] <<" ";
                                }
                                ++row_num;
                                cout<<'\n';
                            }
                        }
                    }
                    break;
                }
                case '<':
                {
                    if(!hashmap[filename].bstkey.empty() && hashmap[filename].conditional_column==column_name){
                        auto high = hashmap[filename].bstkey.lower_bound(entry);
                        for(auto it=hashmap[filename].bstkey.begin();it!=high;++it){
                            for(size_t i = 0; i<it->second.size(); ++i){
                                for(size_t j =0; j<indices.size(); ++j){
                                    cout << hashmap[filename].table[it->second[i]][indices[j]] <<" ";
                                }
                                ++row_num;
                                cout<<'\n';
                            }
                        }
                    }
                    else{
                        for(size_t i = 0 ; i <hashmap[filename].table.size();++i){
                            if(hashmap[filename].table[i][index] < entry){
                                for(size_t j =0; j<indices.size(); ++j){
                                    cout << hashmap[filename].table[i][indices[j]] <<" ";
                                }
                                ++row_num;
                                cout<<'\n';
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
    cout<<"Printed "<< row_num<< " matching rows from "<<filename<<'\n';
}

bool Program::three_helper(string filename, char op, TableEntry entry, size_t index){
    switch(op){
        case '=':
        {
            size_t original_size =hashmap[filename].table.size();
            auto it = remove_if(hashmap[filename].table.begin(), hashmap[filename].table.end(), Equals(entry, index));
            size_t deleted =static_cast<size_t>(hashmap[filename].table.end()-it);
            cout<<"Deleted "<<deleted<<" rows from "<<filename<<'\n';
            hashmap[filename].table.resize(original_size-deleted);
            break;
        }
        case '>':
        {
            size_t original_size =hashmap[filename].table.size();
            auto it =remove_if(hashmap[filename].table.begin(), hashmap[filename].table.end(),Greater(entry, index));
            size_t deleted =static_cast<size_t>(hashmap[filename].table.end()-it);
            cout<<"Deleted "<<deleted<<" rows from "<<filename<<'\n';
            hashmap[filename].table.resize(original_size-deleted);
            break;
        }
        case '<':
        {
            size_t original_size =hashmap[filename].table.size();
            auto it =remove_if(hashmap[filename].table.begin(), hashmap[filename].table.end(),Less(entry, index));
            size_t deleted =static_cast<size_t>(hashmap[filename].table.end()-it);
            cout<<"Deleted "<<deleted<<" rows from "<<filename<<'\n';
            hashmap[filename].table.resize(original_size-deleted);
            break;
        }
    }
    return false;
}

void Program::run_delete(){
    string filename;
    string c;
    string column_name;
    char op;
    cin>>c>>filename>>c>>column_name>>op;
    if(hashmap.find(filename)==hashmap.end()){
        cout<<"Error during DELETE: "<<filename<<" does not name a table in the database\n";
        getline(cin,c);
        return;
    }
//    cout<<filename<<column_name<<op;
    size_t index = find_index(filename, column_name);
    if(index==hashmap[filename].column_name.size()){
        cout<<"Error during DELETE: "<<column_name<<" does not name a column in "<<filename<<'\n';
        getline(cin,c);
        return;
    }
    
//    auto it = find(current.column_name.begin(),current.column_name.end(), column_name);
//    size_t index = it - current.column_name.begin();
    TableEntry entry = four_helper(filename, index);
    three_helper(filename, op, entry, index);
    
    if(!hashmap[filename].hashkey.empty() && hashmap[filename].type=='h'){
        size_t index = find_index(filename,hashmap[filename].conditional_column);
        hashmap[filename].hashkey.clear();
        for(size_t i = 0 ; i < hashmap[filename].table.size(); ++i){
            TableEntry key =hashmap[filename].table[i][index];
            hashmap[filename].hashkey[key].push_back(i);
        }
    }
    else if(!hashmap[filename].bstkey.empty()&& hashmap[filename].type=='b'){
        hashmap[filename].bstkey.clear();
        size_t index = find_index(filename,hashmap[filename].conditional_column);
        for(size_t i = 0 ; i < hashmap[filename].table.size(); ++i){
            TableEntry key =hashmap[filename].table[i][index];
            hashmap[filename].bstkey[key].push_back(i);
        }
    }
    

}

size_t Program::find_index(string filename, string column_name){
    auto it = find(hashmap[filename].column_name.begin(),hashmap[filename].column_name.end(), column_name);
    size_t index = static_cast<size_t>(it - hashmap[filename].column_name.begin());
    return index;
}

void Program::run_join(){
    string c;
    size_t num;
    string file_one;
    string file_two;
    string column_name1;
    string column_name2;
    vector<pair<string,size_t>> prints;
    cin>>file_one>>c>>file_two>>c>>column_name1>>c>>column_name2>>c>>c>>num;

    if(hashmap.find(file_one)==hashmap.end()){
        cout<<"Error during JOIN: "<<file_one<<" does not name a table in the database\n";
        getline(cin,c);
        return;
    }
    if(hashmap.find(file_two)==hashmap.end()){
        cout<<"Error during JOIN: "<<file_two<<" does not name a table in the database\n";
        getline(cin,c);
        return;
    }
    size_t index1 = find_index(file_one, column_name1);
    size_t index2 = find_index(file_two, column_name2);
    if(index1==hashmap[file_one].column_name.size()){
        cout<<"Error during JOIN: "<<column_name1<<" does not name a column in "<<file_one<<'\n';
        getline(cin,c);
        return;
    }
    if(index2==hashmap[file_two].column_name.size()){
        cout<<"Error during JOIN: "<<column_name2<<" does not name a column in "<<file_two<<'\n';
        getline(cin,c);
        return;
    }

    
    vector<string> tables{file_one,file_two};
    string col;
    size_t file;
    if(quiet){
        for(size_t i = 0; i<num;++i){
            cin>>col>>file;
            size_t index =find_index(tables[file-1], col);
            prints.push_back(make_pair(tables[file-1], index));
        }
    }
    else{
        for(size_t i = 0; i<num;++i){
            cin>>col>>file;
            cout<<col<<" ";
            size_t index =find_index(tables[file-1], col);
            prints.push_back(make_pair(tables[file-1], index));
        }
        cout<<'\n';
    }
    size_t count =0;
    if(quiet){
        if(hashmap[file_two].conditional_column!=column_name2){
            unordered_map<TableEntry, vector<size_t>> temp_key;
            for(size_t i = 0 ; i < hashmap[file_two].table.size(); ++i){
                TableEntry key =hashmap[file_two].table[i][index2];
                temp_key[key].push_back(i);
            }
            for(size_t i =0; i < hashmap[file_one].table.size();++i){
                if(!temp_key[hashmap[file_one].table[i][index1]].empty()){
                    count+=temp_key[hashmap[file_one].table[i][index1]].size();
                    
                }
            }
        }
        else if(!hashmap[file_two].hashkey.empty()){
            for(size_t i =0; i < hashmap[file_one].table.size();++i){
                if(!hashmap[file_two].hashkey[hashmap[file_one].table[i][index1]].empty()){
                    count+=hashmap[file_two].hashkey[hashmap[file_one].table[i][index1]].size();
                    
                }
            }
        }
        else if(!hashmap[file_two].bstkey.empty()){
            for(size_t i =0; i < hashmap[file_one].table.size();++i){
                if(!hashmap[file_two].bstkey[hashmap[file_one].table[i][index1]].empty()){
                    count+=hashmap[file_two].bstkey[hashmap[file_one].table[i][index1]].size();
                }
            }
        }
    }
    else{
        if(hashmap[file_two].conditional_column!=column_name2){
            unordered_map<TableEntry, vector<size_t>> temp_key;
            for(size_t i = 0 ; i < hashmap[file_two].table.size(); ++i){
                TableEntry key =hashmap[file_two].table[i][index2];
                temp_key[key].push_back(i);
            }
            for(size_t i =0; i < hashmap[file_one].table.size();++i){
                if(!temp_key[hashmap[file_one].table[i][index1]].empty()){
                    for(size_t j=0; j<temp_key[hashmap[file_one].table[i][index1]].size(); ++j){
                        for(pair<string, size_t> p: prints){
                            if(p.first==tables[0]){
                                cout<<hashmap[p.first].table[i][p.second]<<" ";
                            }
                            else{
                                cout<<hashmap[p.first].table[temp_key[hashmap[file_one].table[i][index1]][j]][p.second]<<" ";
                            }
                        }
                        ++count;
                        cout<<'\n';
                    }
                }
            }
        }
        else if(!hashmap[file_two].hashkey.empty()){
            for(size_t i =0; i < hashmap[file_one].table.size();++i){
                if(!hashmap[file_two].hashkey[hashmap[file_one].table[i][index1]].empty()){
                    for(size_t j=0; j<hashmap[file_two].hashkey[hashmap[file_one].table[i][index1]].size(); ++j){
                        for(pair<string, size_t> p: prints){
                            if(p.first==tables[0]){
                                cout<<hashmap[p.first].table[i][p.second]<<" ";
                            }
                            else{
                                cout<<hashmap[p.first].table[hashmap[file_two].hashkey[hashmap[file_one].table[i][index1]][j]][p.second]<<" ";
                            }
                        }
                        ++count;
                        cout<<'\n';
                    }
                }
            }
        }
        else if(!hashmap[file_two].bstkey.empty()){
            for(size_t i =0; i < hashmap[file_one].table.size();++i){
                if(!hashmap[file_two].bstkey[hashmap[file_one].table[i][index1]].empty()){
                    for(size_t j=0; j<hashmap[file_two].bstkey[hashmap[file_one].table[i][index1]].size(); ++j){
                        for(pair<string, size_t> p: prints){
                            if(p.first==tables[0]){
                                cout<<hashmap[p.first].table[i][p.second]<<" ";
                            }
                            else{
                                cout<<hashmap[p.first].table[hashmap[file_two].bstkey[hashmap[file_one].table[i][index1]][j]][p.second]<<" ";
                            }
                        }
                        ++count;
                        cout<<'\n';
                    }
                }
            }
        }
        
    }
    cout<<"Printed "<<count<<" rows from joining "<<tables[0]<<" to "<<tables[1]<<'\n';
}

void Program::run_generate(){
    string filename;
    string c;
    string type;
    string column_name;
    cin>>c>>filename>>type>>c>>c>>column_name;
    if(hashmap.find(filename)==hashmap.end()){
        cout<<"Error during GENERATE: "<<filename<<" does not name a table in the database\n";
        getline(cin,c);
        return;
    }
    size_t index = find_index(filename,column_name);
    if(index==hashmap[filename].column_name.size()){
        cout<<"Error during GENERATE: "<<column_name<<" does not name a column in "<<filename<<'\n';
        getline(cin,c);
        return;
    }
    hashmap[filename].conditional_column = column_name;
    hashmap[filename].bstkey.clear();
    hashmap[filename].hashkey.clear();
    if(type[0]=='h'){
        hashmap[filename].type = 'h';
        for(size_t i = 0 ; i < hashmap[filename].table.size(); ++i){
            TableEntry key =hashmap[filename].table[i][index];
            hashmap[filename].hashkey[key].push_back(i);
        }
        cout<<"Created hash index for table "<<filename<<" on column "<<column_name<<'\n';
    }
    else{
        hashmap[filename].type = 'b';
        for(size_t i = 0 ; i < hashmap[filename].table.size(); ++i){
            TableEntry key =hashmap[filename].table[i][index];
            hashmap[filename].bstkey[key].push_back(i);
        }
        cout<<"Created bst index for table "<<filename<<" on column "<<column_name<<'\n';
    }
}


int main(int argc, char * argv[]) {
    ios_base::sync_with_stdio(false);
    xcode_redirect(argc, argv);
    cin >> std::boolalpha;
    cout << std::boolalpha;
    Program program;
    program.get_mode(argc, argv);
    program.run();
    return 0;
}
