/******************************************************************************
 * $Id:  $
 *
 * Project:   OpenGIS Simple Features for OpenDRIVE format
 * Purpose:   Implements OGRXODRDriver class.
 * Authors:   Ana Maria Orozco  <email>
 *			  Michael Scholz    <email> 
 *            Deutsches Zentrum für Luft- und Raumfahrt, DLR
 ******************************************************************************/

#include "ogr_xodr.h"
#include "ogr_api.h"
#include "cpl_conv.h"
#include "xodr.h"

/************************************************************************/
/*                         OGRXODRDataSource()                          */
/************************************************************************/

OGRXODRDataSource::OGRXODRDataSource()

{
    pszName = NULL;
	papoLayers = NULL;
    nLayers = 0;
	xodr = NULL;
}

/************************************************************************/
/*                        ~OGRXODRDataSource()                          */
/************************************************************************/

OGRXODRDataSource::~OGRXODRDataSource()

{
    for( int i = 0; i < nLayers; i++ )
        delete papoLayers[i];
    CPLFree( papoLayers );
	CPLFree( pszName );
}

/************************************************************************/
/*                              Open()                                  */
/************************************************************************/

//int  OGRXODRDataSource::Open( const char *pszFilename, int bUpdate )
int  OGRXODRDataSource::Open( GDALOpenInfo* poOpenInfo )
{
	int bUpdate = poOpenInfo->eAccess == GA_Update;
	if( bUpdate )
	{
       CPLError( CE_Failure, CPLE_OpenFailed, 
                  "Update access not supported by the XODR driver." );
        return FALSE;
    }


  // -------------------------------------------------------------------- 
  //      Create a corresponding layer.
  // --------------------------------------------------------------------
    const char * pszNewName = poOpenInfo->pszFilename;

	// papszOpenOptions = CSLDuplicate( poOpenInfo->papszOpenOptions );
	
	nLayers = 1  ;

	OGRXODRLayer       *poLayer;

     papoLayers = (OGRXODRLayer **)
        CPLRealloc( papoLayers,  sizeof(OGRXODRLayer *) * (nLayers) );

	papoLayers[0] = new OGRXODRLayer( pszNewName );
		
	pszName = CPLStrdup( pszNewName );

    return TRUE;
}

/************************************************************************/
/*                              GetLayer()                              */
/************************************************************************/

OGRLayer *OGRXODRDataSource::GetLayer( int iLayer )

{
	//GetLayerCount();
    if( iLayer < 0 || iLayer >= nLayers )
        return NULL;
    else
        return papoLayers[iLayer];
}


int OGRXODRDataSource::TestCapability( const char * pszCap )

{
    if( EQUAL(pszCap, ODsCCreateLayer) )
        return TRUE;
    else
        return FALSE;
}