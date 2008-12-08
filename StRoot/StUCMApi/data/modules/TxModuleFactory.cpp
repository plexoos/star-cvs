/*
 * TxModuleFactory.cpp
 *
 * Created on Aug 20, 2007
 *
 * Author: Zhengqiang Liang (Wayne State University)
 *            Valeri Fine (Brookhaven National Laboratory)
 *            Jerome Lauret (Brookhaven National Laboratory)
 *            Stephen Tramer (Tech-X Corp.)
 *            
 *
 * This file is part of the UCM project funded under an SBIR
 * Copyright (c) 2007-2008 STAR Collaboration - Brookhaven National Laboratory
 *
 * @(#)cpp/api:$Id: TxModuleFactory.cpp,v 1.1 2008/12/08 21:10:32 fine Exp $
 *
 *
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * STAR Scheduler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with STAR Scheduler; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
#include <string>
#include <cctype>

#include "TxMySQLModule.h"
#include "TxModuleFactory.h"
#include "TxModuleException.h"

using namespace std;

TxModuleFactory::TxModuleFactory()
{}

TxModule* TxModuleFactory::createModule(const std::string& storageType, 
					const std::string& location) {
  if (location.empty()) {
    throw TxModuleException("Error creating module: No location",
			    TxModuleException::CONNECT);
  }

  if (storageType.empty()) {
    throw TxModuleException("Error creating module: No storage type",
			    TxModuleException::CONNECT);
  }
  
  if(storageType == "mysql") {
    return new TxMySQLModule(location);
  }
  else { 
    throw TxModuleException("Error creating module: No module type '" 
			     + storageType + "'",
			    TxModuleException::CONNECT);
  }
}

TxModule* TxModuleFactory::createModule(const std::string& type) {
  if (type.empty()) {
    throw TxModuleException("Error creating module: No module type/location "
			    "provided.",
			    TxModuleException::CONNECT);
  }
  
  size_t splitToken = type.find(':');
  std::string storageType = type.substr(0,splitToken);
  std::string storageLocation = type.substr(splitToken+1);
  
  std::transform(storageType.begin(), storageType.end(), 
		 storageType.begin(), (int(*)(int))tolower );
  
  return createModule(storageType, storageLocation);
}
