// -*- C++ -*-
//
// Package:     Core
// Class  :     FWConfigurationManager
//
// Implementation:
//     <Notes on implementation>
//
// Original Author:  Chris Jones
//         Created:  Sun Feb 24 14:42:32 EST 2008
//

// system include files
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include "TROOT.h"

// user include files
#include "Fireworks/Core/interface/FWConfigurationManager.h"
#include "Fireworks/Core/interface/FWConfiguration.h"
#include "Fireworks/Core/interface/FWConfigurable.h"
#include "Fireworks/Core/interface/fwLog.h"
#include "Fireworks/Core/src/SimpleSAXParser.h"
#include "Fireworks/Core/interface/FWXMLConfigParser.h"

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
FWConfigurationManager::FWConfigurationManager()
{
}

// FWConfigurationManager::FWConfigurationManager(const FWConfigurationManager& rhs)
// {
//    // do actual copying here;
// }

FWConfigurationManager::~FWConfigurationManager()
{
}

//
// assignment operators
//
// const FWConfigurationManager& FWConfigurationManager::operator=(const FWConfigurationManager& rhs)
// {
//   //An exception safe implementation is
//   FWConfigurationManager temp(rhs);
//   swap(rhs);
//
//   return *this;
// }

//
// member functions
//
void
FWConfigurationManager::add(const std::string& iName, FWConfigurable* iConf)
{
   assert(0!=iConf);
   m_configurables[iName]=iConf;
}

//
// const member functions
//
void
FWConfigurationManager::setFrom(const FWConfiguration& iConfig) const
{
   assert(0!=iConfig.keyValues());
   printf("FWConfigurationManager::setFrom\n");
   for(FWConfiguration::KeyValues::const_iterator it = iConfig.keyValues()->begin(),
                                                  itEnd = iConfig.keyValues()->end();
       it != itEnd;
       ++it) {
      std::map<std::string,FWConfigurable*>::const_iterator itFound = m_configurables.find(it->first);
      std::cerr << "==== " << it->first <<std::endl;
      assert(itFound != m_configurables.end());
      itFound->second->setFrom(it->second);
   }
}

void
FWConfigurationManager::to(FWConfiguration& oConfig) const
{
   FWConfiguration config;
   for(std::map<std::string,FWConfigurable*>::const_iterator it = m_configurables.begin(),
                                                             itEnd = m_configurables.end();
       it != itEnd;
       ++it) {
      it->second->addTo(config);
      oConfig.addKeyValue(it->first, config, true);
   }
}


void
FWConfigurationManager::writeToFile(const std::string& iName) const
{
   try
   {
      std::ofstream file(iName.c_str());
      if(not file) {
         std::string message = "unable to open file " + iName;
         message += iName;
         throw std::runtime_error(message.c_str());
      }
      FWConfiguration top;
      to(top);
      fwLog(fwlog::kInfo) << "Writing to file "<< iName.c_str() << "...\n";
      fflush(stdout);

      streamTo(file, top, "top");
   }
   catch (std::runtime_error &e)
   { 
      fwLog(fwlog::kError) << "FWConfigurationManager::writeToFile() " << e.what() << std::endl;
   }
}

void
FWConfigurationManager::readFromOldFile(const std::string& iName) const
{
   Int_t error=0;
   // Int_t value =
   gROOT->LoadMacro( iName.c_str(), &error );
   if(0 != error) {
      std::string message("unable to load macro file ");
      message += iName;
      throw std::runtime_error(message.c_str());
   }

   const std::string command("(Long_t)(fwConfig() )");

   error = 0;
   Long_t lConfig = gROOT->ProcessLineFast(command.c_str(),
                                           &error);

   {
      //need to unload this macro so that we can load a new configuration
      // which uses the same function name in the macro
      Int_t error = 0;
      gROOT->ProcessLineSync((std::string(".U ")+iName).c_str(), &error);
   }
   if(0 != error) {
      std::string message("unable to properly parse configuration file ");
      message += iName;
      throw std::runtime_error(message.c_str());
   }
   std::auto_ptr<FWConfiguration> config( reinterpret_cast<FWConfiguration*>(lConfig) );

   setFrom( *config);
}


/** Reads the configuration specified in @a iName and creates the internal 
    representation in terms of FWConfigutation objects.
    
    Notice that if the file does not start with '<' the old CINT macro based
    system is used.
  */
void
FWConfigurationManager::readFromFile(const std::string& iName) const
{
   std::ifstream f(iName.c_str());
   if (f.peek() != (int) '<')
      return readFromOldFile(iName);
   
   // Check that the syntax is correct.
   SimpleSAXParser syntaxTest(f);
   syntaxTest.parse();
   f.close();
   
   // Read again, this time actually parse.
   std::ifstream g(iName.c_str());
   // Actually parse the results.
   FWXMLConfigParser parser(g);
   parser.parse();
   setFrom(*parser.config());
}

//
// static member functions
//
