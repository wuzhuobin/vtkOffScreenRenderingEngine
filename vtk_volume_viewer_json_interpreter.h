#ifndef __VTK_VOLUME_VIEWER_JSON_INTERPRETER_H__
#define __VTK_VOLUME_VIEWER_JSON_INTERPRETER_H__
#pragma once
// me 
#include "vtkoffscreenrenderingengine_export.h"
class vtkVolumeViewer;
class vtkRenderWindowInteractor;
class vtkInteractorStyleTrackballCamera;
//boos
#include <boost/property_tree/json_parser.hpp>
// std
#include <string>
#include <iostream>
#include <array>
class VTKOFFSCREENRENDERINGENGINE_DEPRECATED_EXPORT vtk_volume_viewer_json_interpreter final
{
public:
  vtk_volume_viewer_json_interpreter(bool debug = false);
  ~vtk_volume_viewer_json_interpreter();
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
  void dolly(vtkRenderWindowInteractor *interactorer) const;
  void pan(vtkRenderWindowInteractor *interactorer) const;
  void spin(vtkRenderWindowInteractor *interactorer) const;
  void rotate(vtkRenderWindowInteractor *interactorer) const;
  template<typename T>
  bool get_value(const std::string &key, T &value) const;
  template<typename T, std::size_t N>
  bool get_values(const std::string &key, std::array<T, N>& values) const;
  vtk_volume_viewer_json_interpreter(const vtk_volume_viewer_json_interpreter &) = delete;
  void operator=(const vtk_volume_viewer_json_interpreter &) = delete;
  vtk_volume_viewer_json_interpreter(vtk_volume_viewer_json_interpreter &&) = delete;
  void operator=(vtk_volume_viewer_json_interpreter &&) = delete;
  boost::property_tree::ptree content;
  bool debug;
  vtkInteractorStyleTrackballCamera *style;
};

#endif // !__VTK_VOLUME_VIEWER_JSON_INTERPRETER_H__