// Copyright 2013 Trimble Navigation Limited. All Rights Reserved.

#ifndef SKPTOXML_WIN32_XMPLUGINWIN_H
#define SKPTOXML_WIN32_XMPLUGINWIN_H

#include "../plugin/xmlplugin.h"

class CXmlExporterPluginWin : public CXmlExporterPlugin {
 public:
  static CXmlExporterPluginWin* GetInstance();
  static void DestroyInstance();

  CXmlExporterPluginWin();
  virtual ~CXmlExporterPluginWin() {
  }

  // Most of the plugin interface is provided by our base class but these
  // two functions perform UI.
  void ShowOptionsDialog(bool model_has_selection);

 protected:
  void ShowSummaryDialog(const std::string& summary);

  static CXmlExporterPluginWin* s_pInstance;
};

#endif  // SKPTOXML_WIN32_XMPLUGINWIN_H
