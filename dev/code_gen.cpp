/*
	File: code_gen.cpp
	Name: Juan Bancamper
	Class: CMPT 432
	Date: 25 Apr 2016

	This file defines all the functions for the class CodeGen.
*/

#include "bin/code_gen.h"
#include <iostream>
#include <sstream>
#include <regex>

CodeGen::CodeGen(){
	heap_ptr = 255;
	code_ptr = 0;
	temp_num = 0;
	scope_level = -1;
	compilation_temp_vars = 0;
	jump_num = 0;
}

CodeGen::~CodeGen(){

}

node *CodeGen::get_parent(node *cur){
	if(cur -> older_sibling){
		return get_parent(cur -> older_sibling);
	}

	return cur -> parent;
}

void CodeGen::load_acc_const(std::string constant){
	if((code_ptr + 2) < heap_ptr){
		bytes[code_ptr++] = "A9";
		bytes[code_ptr++] = constant;
	}
	else{
		std::cout << "\nRan out of memory" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void CodeGen::load_acc_mem(std::string addr){
	if((code_ptr + 3) < heap_ptr){
		bytes[code_ptr++] = "AD";
		bytes[code_ptr++] = addr.substr(0, 2);
		bytes[code_ptr++] = addr.substr(2, 2);
	}
	else{
		std::cout << "\nRan out of memory" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void CodeGen::store_acc(std::string addr){
	if((code_ptr + 3) < heap_ptr){
		bytes[code_ptr++] = "8D";
		bytes[code_ptr++] = addr.substr(0, 2);
		bytes[code_ptr++] = addr.substr(2, 2);
	}
	else{
		std::cout << "\nRan out of memory" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void CodeGen::add_with_carry(std::string addr){
	if((code_ptr + 3) < heap_ptr){
		bytes[code_ptr++] = "6D";
		bytes[code_ptr++] = addr.substr(0, 2);
		bytes[code_ptr++] = addr.substr(2, 2);
	}
	else{
		std::cout << "\nRan out of memory" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void CodeGen::load_x_const(std::string constant){
	if((code_ptr + 2) < heap_ptr){
		bytes[code_ptr++] = "A2";
		bytes[code_ptr++] = constant;
	}
	else{
		std::cout << "\nRan out of memory" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void CodeGen::load_x_mem(std::string addr){
	if((code_ptr + 3) < heap_ptr){
		bytes[code_ptr++] = "AE";
		bytes[code_ptr++] = addr.substr(0, 2);
		bytes[code_ptr++] = addr.substr(2, 2);
	}
	else{
		std::cout << "\nRan out of memory" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void CodeGen::load_y_const(std::string constant){
	if((code_ptr + 2) < heap_ptr){
		bytes[code_ptr++] = "A0";
		bytes[code_ptr++] = constant;
	}
	else{
		std::cout << "\nRan out of memory" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void CodeGen::load_y_mem(std::string addr){
	if((code_ptr + 3) < heap_ptr){
		bytes[code_ptr++] = "AC";
		bytes[code_ptr++] = addr.substr(0, 2);
		bytes[code_ptr++] = addr.substr(2, 2);
	}
	else{
		std::cout << "\nRan out of memory" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void CodeGen::no_op(){
	bytes[code_ptr++] = "EA";
}

void CodeGen::break_program(){
	bytes[code_ptr++] = "00";
}

void CodeGen::compare_x_to_mem(std::string addr){
	if((code_ptr + 3) < heap_ptr){
		bytes[code_ptr++] = "EC";
		bytes[code_ptr++] = addr.substr(0, 2);
		bytes[code_ptr++] = addr.substr(2, 2);
	}
	else{
		std::cout << "\nRan out of memory" << std::endl;
		exit(EXIT_FAILURE);
	}
}

std::string CodeGen::branch_not_equal(){
	bytes[code_ptr++] = "D0";

	std::stringstream temp_addr;
	temp_addr << "J" << jump_num++;

	jump_table.insert(std::pair<std::string, int>(temp_addr.str(), 0));

	bytes[code_ptr++] = temp_addr.str();

	return temp_addr.str();
}

std::string CodeGen::branch_if_equal(){
	bytes[code_ptr++] = "F0";

	std::stringstream temp_addr;
	temp_addr << "J" << jump_num++;

	jump_table.insert(std::pair<std::string, int>(temp_addr.str(), 0));

	bytes[code_ptr++] = temp_addr.str();

	return temp_addr.str();
}

void CodeGen::increment(std::string addr){
	if((code_ptr + 3) < heap_ptr){
		bytes[code_ptr++] = "EE";
		bytes[code_ptr++] = addr.substr(0, 2);
		bytes[code_ptr++] = addr.substr(2, 2);
	}
	else{
		std::cout << "\nRan out of memory" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void CodeGen::system_call(){
	bytes[code_ptr++] = "FF";
}

void CodeGen::print_tables(){
	std::cout << "\n\n\t\t\tStatic Data\n" 
		<< "  Temp Addr   |     Var    |    Scope   |   Offset\n" 
		<< "----------------------------------------------------" 
		<< std::endl;

	for (std::vector<var_addr>::iterator i = static_table.begin(); 
		i != static_table.end(); 
		i++){
		
		std::cout << "     " << i -> temp << "     |      " << i -> variable
			<< "     |      " << i -> scope << "     |      "
			<< i -> offset << std::endl;
	}

	std::cout << "\n\n\tJump Table\n" 
		<< "  Temp Addr   |   Offset\n" 
		<< "--------------------------" 
		<< std::endl;

	for (std::map<std::string, int>::iterator i = jump_table.begin();
		i != jump_table.end();
		i++){
		
		std::cout << "      " << i -> first << "      |     " 
			<< i -> second << std::endl;	
	}
}

void CodeGen::print_hex(){
	std::cout << std::endl;
	for (int i = 0; i < 256; i++){
		if(i % 8 == 0 && i > 0){
			std::cout << std::endl;
		}
		std::cout << bytes[i] << " ";
	}
	std::cout << std::endl;
}

struct var_addr CodeGen::find_var(char c){
	for (std::vector<var_addr>::iterator i = static_table.end() - 1;
		i != static_table.begin() - 1; 
		i--){

		if((i -> variable == c) && (i -> scope <= scope_level)){
			return *i;
		}
	}
}

struct var_addr CodeGen::find_var(std::string addr){
	for (std::vector<var_addr>::iterator i = static_table.begin(); 
		i != static_table.end();
		i++){
		
		if(!addr.compare(i -> temp)){
			return *i;
		}
	}
}

std::map<std::string, int>::iterator CodeGen::find_jump(std::string jump){
	for (std::map<std::string, int>::iterator i = jump_table.begin(); 
		i != jump_table.end();
		i++){
		
		if(!jump.compare(i -> first)){
			return i;
		}
	}
}

void CodeGen::generate_primitive(std::string type, node *cur){
	std::stringstream temp_addr;
	temp_addr << "T" << temp_num << "xx";

	var_addr v = {
		temp_addr.str(),
		cur -> data[0],
		type,
		scope_level,
		temp_num++
	};

	static_table.push_back(v);

	load_acc_const("00");
	store_acc(v.temp);
}

void CodeGen::generate_string(std::string assigned_string){
	if(std::regex_match(assigned_string, std::regex("\"[a-z ]*\""))){	
		bytes[heap_ptr--] = "00";
		char c = '\0';
		int ascii = 0;
		char *val_star = new char[2];

		for (int i = assigned_string.size() - 2; i > 0; i--){
			c = assigned_string[i];
			ascii = (int)c;

			std::sprintf(val_star, "%02X", ascii);
			bytes[heap_ptr--] = val_star;
		}

		std::sprintf(val_star, "%02X", heap_ptr + 1);
		load_acc_const(val_star);
	}
	else{
		load_acc_mem(find_var(assigned_string[0]).temp);
	}
}

void CodeGen::generate_int(node *cur){
	if(cur){
		if(!cur -> data.compare("add")){
			generate_int(cur -> child -> younger_sibling);
			
			node *n = get_parent(cur);
			if(n && !n -> data.compare("add")){
				std::stringstream temp_addr;
				temp_addr << "I" << compilation_temp_vars++ << "xx";

				var_addr v = {
					temp_addr.str(),
					'#',
					"digit",
					scope_level,
					temp_num++
				};

				static_table.push_back(v);

				store_acc(v.temp);

				char *val_star = new char[2];
				std::sprintf(val_star, "%02X", std::stoi(cur -> older_sibling -> data));

				load_acc_const(val_star);
				add_with_carry(static_table.back().temp);

			}
		}

		else if(std::regex_match(cur -> data, std::regex("[0-9]"))){
			char *val_star = new char[2];
			std::sprintf(val_star, "%02X", std::stoi(cur -> data));

			load_acc_const(val_star);

			std::stringstream temp_addr;
			temp_addr << "I" << compilation_temp_vars++ << "xx";

			var_addr v = {
				temp_addr.str(),
				'#',
				"digit",
				scope_level,
				temp_num++
			};

			static_table.push_back(v);

			node *n = get_parent(cur);
			if(n && !n -> data.compare("add")){
				store_acc(v.temp);

				std::sprintf(val_star, "%02X", std::stoi(cur -> older_sibling -> data));
				load_acc_const(val_star);

				add_with_carry(static_table.back().temp);
			}
		}

		else if(std::regex_match(cur -> data, std::regex("[a-z]"))){
			node *n = get_parent(cur);
			if(n && !n -> data.compare("add")){
				char *val_star = new char[2];
				std::sprintf(val_star, "%02X", std::stoi(cur -> older_sibling -> data));

				load_acc_const(val_star);
				add_with_carry(find_var(cur -> data[0]).temp);
			}
			else{
				load_acc_mem(find_var(cur -> data[0]).temp);
			}
		}
	}
}

std::string CodeGen::generate_bool(node *cur){
	if(cur) {
		if(!cur -> data.compare("==")){
			std::string expr_1 = generate_bool(cur -> child);
			std::string expr_2 = generate_bool(cur -> child -> younger_sibling);

			if(std::regex_match(expr_1, std::regex("T[0-9]*xx")) &&
				std::regex_match(expr_2, std::regex("T[0-9]*xx"))){

				load_x_mem(expr_1);
				compare_x_to_mem(expr_2);
			}
			else if(std::regex_match(expr_1, std::regex("T[0-9]*xx"))){
				char *val_star = new char[2];
		 		std::sprintf(val_star, "%02X", std::stoi(expr_2));

				load_x_const(val_star);
				compare_x_to_mem(expr_1);
			}
			else if(std::regex_match(expr_2, std::regex("T[0-9]*xx"))) {
				char *val_star = new char[2];
		 		std::sprintf(val_star, "%02X", std::stoi(expr_1));

				load_x_const(val_star);
				compare_x_to_mem(expr_2);
			}
			else{
				// load_acc_const(expr_1);

			}

			return "";
		}
		else if (!cur -> data.compare("!=")){
			std::string expr_1 = generate_bool(cur -> child);
			std::string expr_2 = generate_bool(cur -> child -> younger_sibling);

			if(std::regex_match(expr_1, std::regex("T[0-9]*xx")) &&
				std::regex_match(expr_2, std::regex("T[0-9]*xx"))){

				
			}
			else if(std::regex_match(expr_1, std::regex("T[0-9]*xx"))){
				char *val_star = new char[2];
		 		std::sprintf(val_star, "%02X", std::stoi(expr_2));

				load_x_const(val_star);
				compare_x_to_mem(expr_1);
			}
			else if(std::regex_match(expr_2, std::regex("T[0-9]*xx"))) {
				char *val_star = new char[2];
		 		std::sprintf(val_star, "%02X", std::stoi(expr_1));

		 		std::stringstream temp_addr;
				temp_addr << "I" << compilation_temp_vars++ << "xx";

				var_addr v = {
					temp_addr.str(),
					'*',
					"expr",
					scope_level,
					temp_num++
				};

				static_table.push_back(v);

				load_acc_const(val_star);
				store_acc(v.temp);

				load_x_mem(v.temp);
				compare_x_to_mem(expr_2);
				increment(v.temp);

				bytes[code_ptr++] = "D0";
				bytes[code_ptr++] = "06";

				load_x_mem(v.temp);
				compare_x_to_mem(expr_2);
			}
			else{
				// load_acc_const(expr_1);

			}

			return "";
		}
		else if(!cur -> data.compare("true") ||
			!cur -> data.compare("false")){

			return (!cur -> data.compare("false") ? "00" : "01");
		}
		else if(std::regex_match(cur -> data, std::regex("[0-9]")) ||
			std::regex_match(cur -> data, std::regex("\"[a-z ]*\""))){

			if(std::regex_match(cur -> data, std::regex("[0-9]"))) {
				return cur -> data;
			}
			else{


				return "";
			}
		}
		else if(std::regex_match(cur -> data, std::regex("[a-z]"))){
			return find_var(cur -> data[0]).temp;
		}
	}
}

void CodeGen::generate_var_decl(node *cur){
	std::string type = cur -> data;

	if(!type.compare("int") || !type.compare("boolean")){
		generate_primitive(type, cur -> younger_sibling);
	}
	else{ //string
		std::stringstream temp_addr;
		temp_addr << "T" << temp_num << "xx";

		var_addr v = {
			temp_addr.str(),
			cur -> younger_sibling -> data[0],
			type,
			scope_level,
			temp_num++
		};

		static_table.push_back(v);
	}
}

void CodeGen::generate_assignment(node *cur){
	if(cur){
		char c = cur -> data[0];

		var_addr v = find_var(c);

		if(!v.type.compare("int")){
			generate_int(cur -> younger_sibling);
		}
		else if(!v.type.compare("boolean")){
			std::string a = generate_bool(cur -> younger_sibling);

			load_acc_const(a);
		}
		else if(!v.type.compare("string")){
			generate_string(cur -> younger_sibling -> data);
		}

		store_acc(v.temp);
	}
}

std::string CodeGen::generate_if(node *cur){
	if(cur){
		std::string a = generate_bool(cur);

		if(!a.empty()){
			std::stringstream temp_addr;
			temp_addr << "I" << compilation_temp_vars++ << "xx";

			var_addr v = {
				temp_addr.str(),
				'#',
				"digit",
				scope_level,
				temp_num++
			};

			if(std::stoi(a)){
				static_table.push_back(v);
				load_acc_const(a);
			}
			else{
				load_acc_const("01");
			}

			store_acc(v.temp);

			load_x_const(a);
			compare_x_to_mem(v.temp);

		}

		return branch_not_equal();
	}
}

std::string CodeGen::generate_while(node *cur){
	if(cur){
		std::string a = generate_bool(cur);

		if(!a.empty()){
			std::stringstream temp_addr;
			temp_addr << "I" << compilation_temp_vars++ << "xx";

			var_addr v = {
				temp_addr.str(),
				'#',
				"digit",
				scope_level,
				temp_num++
			};

			if(std::stoi(a)){
				static_table.push_back(v);
				load_acc_const("00");
			}
			else{
				load_acc_const(a);
			}

			store_acc(v.temp);

			load_x_const(a);
			compare_x_to_mem(v.temp);


		}

		return branch_not_equal();
	}
}

void CodeGen::generate_print(node *cur){
	if(cur){
		if(!cur -> data.compare("add")){
			generate_int(cur);

			std::stringstream temp_addr;
			temp_addr << "I" << compilation_temp_vars++ << "xx";

			var_addr v = {
				temp_addr.str(),
				'#',
				"digit",
				scope_level,
				temp_num++
			};

			static_table.push_back(v);

			store_acc(v.temp);

			load_y_mem(v.temp);
			load_x_const("01");
		}
		else if(std::regex_match(cur -> data, std::regex("[0-9]"))) {
			char *val_star = new char[2];
	 		std::sprintf(val_star, "%02X", std::stoi(cur -> data));

			load_y_const(val_star);
			load_x_const("01");
		}
		else if(std::regex_match(cur -> data, std::regex("\"[a-z ]*\""))){
			generate_string(cur -> data);

			std::stringstream temp_addr;
			temp_addr << "I" << compilation_temp_vars++ << "xx";

			var_addr v = {
				temp_addr.str(),
				'\"',
				"string",
				scope_level,
				temp_num++
			};

			static_table.push_back(v);

			store_acc(v.temp);

			load_y_mem(v.temp);
			load_x_const("02");
		}
		else if(std::regex_match(cur -> data, std::regex("[a-z]"))){
			load_y_mem(find_var(cur -> data[0]).temp);
			
			if(!find_var(cur -> data[0]).type.compare("string")){
				load_x_const("02");
			}
			else{
				load_x_const("01");
			}
		}

		system_call();
	}
}

void CodeGen::generate(node *cur){
	if(cur){
		int a = 0, bool_jump = 0;
		std::string jump = "";

		if(!cur -> data.compare("block")){
			scope_level++;
		}
		else if(!cur -> data.compare("var_decl")){
			generate_var_decl(cur -> child);
		}
		else if(!cur -> data.compare("assignment")){
			generate_assignment(cur -> child);
		}
		else if(!cur -> data.compare("if")){
			jump = generate_if(cur -> child);
			a = code_ptr;
		}
		else if(!cur -> data.compare("while")){
			bool_jump = code_ptr + 1;
			a = code_ptr;
		}
		else if(!cur -> data.compare("print")){
			generate_print(cur -> child);
		}

		generate(cur -> child);

		if(!cur -> data.compare("block")){
			scope_level--;
		}
		else if(!cur -> data.compare("if")){
			std::map<std::string, int>::iterator j = find_jump(jump);
			j -> second = code_ptr - a;

			// std::cout << "If Jump Distance: " << j -> second << std::endl;
		}
		else if(!cur -> data.compare("while")){
			jump = generate_while(cur -> child);

			std::map<std::string, int>::iterator j = find_jump(jump);

			j -> second = 256 - (code_ptr - bool_jump) - 1;
		}

		generate(cur -> younger_sibling);
	}
}

void CodeGen::back_patch(){
	break_program();

	for (int i = 0; i < code_ptr; i++){
		if(std::regex_match(bytes[i], std::regex("T[0-9]*"))){

			int offset = find_var(bytes[i] + bytes[i + 1]).offset;
			char *val_star = new char[2];
	 		std::sprintf(val_star, "%02X", code_ptr + offset);

	 		bytes[i++] = val_star;
	 		bytes[i] = "00";
		}
		else if(std::regex_match(bytes[i], std::regex("I[0-9]*"))){
			int offset = find_var(bytes[i] + bytes[i + 1]).offset;
			char *val_star = new char[2];
	 		std::sprintf(val_star, "%02X", code_ptr + offset);

	 		bytes[i++] = val_star;
	 		bytes[i] = "00";
		}
		else if(std::regex_match(bytes[i], std::regex("J[0-9]*"))){
			int offset = find_jump(bytes[i]) -> second;
			char *val_star = new char[2];
	 		std::sprintf(val_star, "%02X", offset);

	 		// std::cout << bytes[i] << ": " << offset << std::endl;
	 		bytes[i++] = val_star;
		}
	}
}

void CodeGen::fill(){
	while(code_ptr <= heap_ptr){
		bytes[code_ptr++] = "00";
	}
}

CodeGen hex(AST &ast){
	CodeGen cg;
	cg.generate(ast.get_root());
	cg.back_patch();
	cg.fill();
	cg.print_hex();
	cg.print_tables();

	return cg;
}