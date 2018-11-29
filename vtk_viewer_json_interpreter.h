#ifndef __VTK_VOLUME_VIEWER_JSON_INTERPRETER_H__
#define __VTK_VOLUME_VIEWER_JSON_INTERPRETER_H__
#pragma once
// me 
#include "vtkoffscreenrenderingengine_export.h"
class vtkVolumeViewer;
class vtkRenderWindowInteractor;
//boos
#include <boost/property_tree/json_parser.hpp>
// std
#include <string>
#include <iostream>
#include <array>
class VTKOFFSCREENRENDERINGENGINE_EXPORT vtk_viewer_json_interpreter final
{
public:
  vtk_viewer_json_interpreter(bool debug = false);
  ~vtk_viewer_json_interpreter();
  void read_json(const std::string &content);
  void read_json_file(const std::string &filename);
  void write_json(std::string &content);
  void write_json_file(const std::string &filename);
  /**
   * @brief Function for interpreter and bridge. 
   * 
   */
  void interpret(vtkVolumeViewer *viewer) const;
  void interpret(vtkRenderWindowInteractor *interactor) const;
private:
  template<typename T>
  bool get_value(const std::string &key, T &value) const;
  template<typename T, std::size_t N>
  bool get_values(const std::string &key, std::array<T, N>& values) const;
  vtk_viewer_json_interpreter(const vtk_viewer_json_interpreter &) = delete;
  void operator=(const vtk_viewer_json_interpreter &) = delete;
  vtk_viewer_json_interpreter(vtk_viewer_json_interpreter &&) = delete;
  void operator=(vtk_viewer_json_interpreter &&) = delete;
  boost::property_tree::ptree content;
  bool debug;
};

#endif // !__VTK_VOLUME_VIEWER_JSON_INTERPRETER_H__