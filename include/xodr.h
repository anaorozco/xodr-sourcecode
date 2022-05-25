/******************************************************************************
 * $Id:  $
 *
 * Project:   OpenGIS Simple Features for OpenDRIVE format
 * Purpose:   Implements OGRXODRDriver class.
 * Authors:   Ana Maria Orozco  <email>
 *			  Michael Scholz    <email> 
 *            Deutsches Zentrum für Luft- und Raumfahrt, DLR
 ******************************************************************************/


#ifndef _XODR_H_INCLUDED
#define _XODR_H_INCLUDED

#include "OpenDRIVE_1.4E-DRAFT.hxx"
#include "ogr_geometry.h"
#include <cmath>
#include <iostream>
#include <memory>   
#include <string>
#include <typeinfo>
#include <vector>


/************************************************************************/
/*                               XODR                                   */
/************************************************************************/

class XODR 
{
	int				  nummer;
	char             *pszName;
	int				  minorRevision;
	int				  numRoads;
	int				  numSignals;
	std::string		  geoReference;
	

 public:
	 std::auto_ptr<OpenDRIVE>	op;
	 XODR( const char *pszFilename );
    
	~XODR();

	void				roadsTest();
	int					getMinorRevision();
	const char*			getGeoreference(); 
	std::vector<OGRMultiLineString*>  getFeatMultiLineString();
	OGRLineString		lineToLinestring(geometry &geoParam); 
	OGRLineString		arcToLinestring(geometry &geoParam);
	int					getNumberOfRoads();
	int					getNumberOfSignals();
	std::string			getGeoReferenceString();

};



#endif /* ndef _XODR_H_INCLUDED */