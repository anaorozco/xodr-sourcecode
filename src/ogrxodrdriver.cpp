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
#include "cpl_conv.h"
#include "cpl_error.h"

extern "C" void RegisterOGRXODR();

///************************************************************************/
///*                         OGRXODRDriverIdentify()                       */
///************************************************************************/

static int OGRXODRDriverIdentify( GDALOpenInfo* poOpenInfo )
{
  // -------------------------------------------------------------------- 
  //      Does this appear to be an .xodr file?                           
  // --------------------------------------------------------------------
    return EQUAL( CPLGetExtension(poOpenInfo->pszFilename), "xodr" );
}


///************************************************************************/
///*                                Open()                                */
///************************************************************************/

static GDALDataset *OGRXODRDriverOpen( GDALOpenInfo* poOpenInfo )
{
    if( !OGRXODRDriverIdentify(poOpenInfo) )
        return NULL;

    OGRXODRDataSource   *poDS = new OGRXODRDataSource();
    //if( !poDS->Open( poOpenInfo->pszFilename , poOpenInfo->eAccess ) )
	 if( !poDS->Open( poOpenInfo ) )
    {
        delete poDS;
        return NULL;
    }
    else
        return poDS;
}


/************************************************************************/
/*                           RegisterOGRXODR()                          */
/************************************************************************/

void RegisterOGRXODR()
//
{
    GDALDriver  *poDriver;

    if( GDALGetDriverByName( "XODR" ) == NULL )
    {
        poDriver = new GDALDriver();

        poDriver->SetDescription( "XODR" );
        poDriver->SetMetadataItem( GDAL_DCAP_VECTOR, "YES" );
        poDriver->SetMetadataItem( GDAL_DMD_LONGNAME, "OpenDRIVE (XODR) driver" );
        poDriver->SetMetadataItem( GDAL_DMD_EXTENSION, "xodr" );
        poDriver->SetMetadataItem( GDAL_DMD_HELPTOPIC, "drv_xodr.html" );

        poDriver->pfnOpen = OGRXODRDriverOpen;
        poDriver->pfnIdentify = OGRXODRDriverIdentify;
		
        GetGDALDriverManager()->RegisterDriver( poDriver );
    }
}