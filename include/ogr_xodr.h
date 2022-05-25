/******************************************************************************
 * $Id:  $
 *
 * Project:   OpenGIS Simple Features for OpenDRIVE format
 * Purpose:   Implements OGRXODRDriver class.
 * Authors:   Ana Maria Orozco  <email>
 *			  Michael Scholz    <email> 
 *            Deutsches Zentrum für Luft- und Raumfahrt, DLR
 ******************************************************************************/

#ifndef _OGR_XODR_H_INCLUDED
#define _OGR_XODR_H_INCLUDED

#include "ogr_xodr.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"
#include "xodr.h"
#include <vector>


/************************************************************************/
/*                           OGRXODRLayer                                */
/************************************************************************/

class OGRXODRLayer : public OGRLayer
{
    protected:
    
		OGRFeatureDefn*    poFeatureDefn;
	
    VSILFILE		*fp;

	const char*		pszName;

    int				nNextFID;

	int				nCount;

	int				layerId;		

	int				nIterator;

	OGRSpatialReference	*poSRS;

	int				minorRevision; 

	XODR			*xodr;

	bool			moreFeatures;

	std::vector<OGRMultiLineString*>::iterator iterator;  // to iterate over the MultiLine vector

	std::vector<OGRMultiLineString*>	multiLine;

	std::auto_ptr<OpenDRIVE>	op;

 public:
	OGRXODRLayer::OGRXODRLayer( const char* pszName );

   ~OGRXODRLayer();

    void                ResetReading();
    
	OGRFeature *        GetNextFeature();

    OGRFeatureDefn *    GetLayerDefn() { return poFeatureDefn; }
	
    int                 TestCapability( const char * ) { return FALSE; }
			
	int					countFeatures();

	
};



/************************************************************************/
/*                        OGRXODRDataSource                              */
/************************************************************************/

//class OGRXODRDataSource : public OGRDataSource  (old version of gdal)
class OGRXODRDataSource : public GDALDataset 
{
    OGRXODRLayer		**papoLayers;

	int                 nLayers;
	
	char				*pszName;
	
	XODR				*xodr;

	
  public:
                        OGRXODRDataSource();
                        ~OGRXODRDataSource();

    int                 Open( GDALOpenInfo* poOpenInfo );

     int				GetLayerCount() { return nLayers; }
     
	 OGRLayer*			GetLayer( int );
	     
	 int                 TestCapability( const char * );

	  
};




#endif /* ndef _OGR_XODR_H_INCLUDED */