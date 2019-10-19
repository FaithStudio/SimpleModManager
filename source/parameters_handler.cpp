//
// Created by Adrien Blanchet on 16/10/2019.
//

#include "parameters_handler.h"

#include "toolbox.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <switch/services/hid.h>

parameters_handler::parameters_handler() {

  reset();

}
parameters_handler::~parameters_handler() {

  reset();

}

void parameters_handler::initialize() {

  if(not toolbox::do_path_is_file(_parameters_file_path_)){
    recreate_parameters_file(); // create file with default parameters
  }

  read_parameters();
  recreate_parameters_file(); // update version + cleanup

  set_selected_preset_id(0); // default preset selected

}
void parameters_handler::reset() {

  _parameters_file_path_ = "./parameters.ini";
  _data_handler_.clear();
  _presets_list_.clear();
  set_default_parameters();

}

void parameters_handler::set_parameters_file_path(std::string parameters_file_path_) {
  _parameters_file_path_ = parameters_file_path_;
}
void parameters_handler::set_selected_preset_id(int selected_preset_id_){
  if(selected_preset_id_ < 0 or selected_preset_id_ >= int(_presets_list_.size())) _selected_preset_id_ = 0; // reset;
  else _selected_preset_id_ = selected_preset_id_;
  fill_current_preset_parameters();
}

std::string parameters_handler::get_parameter(std::string parameter_name_) {
  return _data_handler_[parameter_name_];
}
std::string parameters_handler::get_parameters_file_path(){
  return _parameters_file_path_;
}
std::string parameters_handler::get_selected_preset_name(){
  return _presets_list_[_selected_preset_id_];
}

void parameters_handler::increment_selected_preset_id(){
  int next_preset_id = _selected_preset_id_+1;
  if(next_preset_id >= int(_presets_list_.size())) set_selected_preset_id(0);
  else set_selected_preset_id(next_preset_id);
}

void parameters_handler::set_default_parameters() {

  _data_handler_["stored-mods-base-folder"] = "/mods/";

  // while adding a new preset dependant parameter
  // don't forget to propagate the changes to "fill_current_preset_parameters()"
  // and "recreate_parameters_file()"
  _presets_list_.emplace_back("default");
  _data_handler_[_presets_list_.back() + "-install-mods-base-folder"] = "/atmosphere/";

  _presets_list_.emplace_back("sxos");
  _data_handler_[_presets_list_.back() + "-install-mods-base-folder"] = "/sxos/";

  _presets_list_.emplace_back("reinx");
  _data_handler_[_presets_list_.back() + "-install-mods-base-folder"] = "/reinx/";

}
void parameters_handler::recreate_parameters_file() {

  std::ofstream parameter_file;
  parameter_file.open (_parameters_file_path_.c_str());

  parameter_file << "# This is a config file" << std::endl;
  parameter_file << std::endl;
  parameter_file << "# folder where mods are stored" << std::endl;
  parameter_file << "stored-mods-base-folder = " << _data_handler_["stored-mods-base-folder"] << std::endl;
  parameter_file << std::endl;
  parameter_file << std::endl;
  for(auto const &preset : _presets_list_){
    parameter_file << "########################################" << std::endl;
    parameter_file << "# preset that can be changed in the app" << std::endl;
    parameter_file << "preset = " << preset << std::endl;
    parameter_file << std::endl;
    parameter_file << "# base folder where mods are installed" << std::endl;
    parameter_file << "install-mods-base-folder = " << _data_handler_[preset + "-install-mods-base-folder"] << std::endl;
    parameter_file << "########################################" << std::endl;
    parameter_file << std::endl;
    parameter_file << std::endl;
  }
  parameter_file << "# DO NOT TOUCH THIS : used to recognise the last version of the program config" << std::endl;
  parameter_file << "last-program-version = " << toolbox::get_app_version() << std::endl;
  parameter_file << std::endl;

  parameter_file.close();


}
void parameters_handler::read_parameters() {

  std::ifstream parameter_file;
  parameter_file.open (_parameters_file_path_.c_str());

  // specifying first preset to keep compatibility with older versions
  // where presets were not implemented yet (1.2.0)
  std::string current_preset = "default";

  std::string line;
  while( std::getline(parameter_file, line) ){

    if(line[0] == '#') continue;

    auto line_elements = toolbox::split_string(line, "=");
    if(line_elements.size() != 2) continue;

    // clean up for extra spaces characters
    for(auto &element : line_elements){
      while(element[0] == ' '){
        element.erase(element.begin());
      }
      while(element[element.size()-1] == ' '){
        element.erase(element.end()-1);
      }
    }

    if(line_elements[0] == "preset"){
      current_preset = line_elements[1];
      append_to_preset_list(current_preset);
    } else {
      _data_handler_[current_preset + "-" + line_elements[0]] = line_elements[1];
    }

  }

  parameter_file.close();

}

void parameters_handler::append_to_preset_list(std::string preset_){

  if (toolbox::do_string_in_vector(preset_, _presets_list_)){
    // preset_ found in _presets_list_
    return;
  } else {
    _presets_list_.emplace_back(preset_);
  }

}
void parameters_handler::fill_current_preset_parameters(){

  _data_handler_["install-mods-base-folder"] = _data_handler_[_presets_list_[_selected_preset_id_] + "-install-mods-base-folder"];

}

