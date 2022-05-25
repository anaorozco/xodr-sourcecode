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
#include "ogr_geometry.h"
#include <cmath>
#include <iostream>
#include <memory>   
#include <string>
#include <typeinfo>
#include <vector>
#include <iterator> 

using namespace std;
using namespace xml_schema;

CPL_CVSID("$Id: ogrxodrlayer.cpp 30000 2015-10-10 11:35:18Z orozco $"); // ?


/************************************************************************/
/*                           OGRXODRLayer()                              */
/************************************************************************/
OGRXODRLayer::OGRXODRLayer( const char* pszName )
{
    nNextFID	  = 0;
	this->pszName = pszName;
	xodr		 =  new XODR( pszName ); 
	nIterator	 = -1;  // non-initialized iterator flag
	
	// -------------------------------------------------------------------- 
	//              Feature definition
	// --------------------------------------------------------------------
	
	poFeatureDefn = new OGRFeatureDefn( CPLGetBasename( pszName ) );
	SetDescription(poFeatureDefn->GetName());
	poFeatureDefn->SetGeomType( wkbMultiLineString );
	poFeatureDefn->Reference();

	multiLine = xodr -> getFeatMultiLineString();

	// -------------------------------------------------------------------- 
	//              Sparial Reference (validation for .xodr 1.4)
	// --------------------------------------------------------------------
 
	 poSRS = new OGRSpatialReference();   
     if( xodr -> getMinorRevision() > 3 ){ 
		 
		 poSRS -> SetWellKnownGeogCS( (xodr->getGeoReferenceString()).c_str() );
		 printf( (xodr->getGeoReferenceString()).c_str() );
	 }

	 // -------------------------------------------------------------------- 
	 //				Fields
	 // --------------------------------------------------------------------

		OGRFieldDefn oFieldName( "name", OFTString );
		poFeatureDefn->AddFieldDefn( &oFieldName );
		
		//OGRFieldDefn oFieldLength( "length", OFTReal );
		//poFeatureDefn->AddFieldDefn( &oFieldLength ); 
		//
		//OGRFieldDefn oFieldId( "id", OFTString );
		//poFeatureDefn->AddFieldDefn( &oFieldId );

		//OGRFieldDefn oFieldJunction( "junction", OFTString );
		//poFeatureDefn->AddFieldDefn( &oFieldJunction );

}

/************************************************************************/
/*                           ~OGRXODRLayer()                             */
/************************************************************************/

OGRXODRLayer::~OGRXODRLayer()

{
	if( NULL != poFeatureDefn )
        poFeatureDefn->Release();

    if( NULL != poSRS )
        poSRS->Release();

	if(xodr)
		delete xodr;
	
}

/************************************************************************/
/*                            ResetReading()                            */
/************************************************************************/

void OGRXODRLayer::ResetReading()

{
	nIterator = -1; 
    nNextFID = 0;
}

/************************************************************************/
/*                          GetFeatureCount()                           */
/************************************************************************/

OGRFeature *OGRXODRLayer::GetNextFeature()
{
	if( nIterator == -1 )
	{
		//return NULL; //--> theorithical break the while, but not!		
		
		iterator  = multiLine.begin();
		nIterator = 0;
		moreFeatures = true;
	} 

	if ( moreFeatures )
	{		
		OGRFeature	*poFeature	= new OGRFeature( poFeatureDefn );  // defined at constructor as MultiLineString
		
		OGRGeometry *poGeom = *iterator;
		//OGRMultiLineString *mul =  *iterator ;
		//OGRGeometry *poGeom = mul;

		//if (poSRS != NULL)
		//	poGeom->assignSpatialReference(poSRS);

		poFeature->SetGeometryDirectly( poGeom );
		
		poFeature->SetField( 0, "road" );
		poFeature->SetFID( nNextFID++ );


		iterator++;	 
		nIterator++;

		if ( iterator == multiLine.end () )
			moreFeatures = false;
				
		
		if( (m_poFilterGeom == NULL
             || FilterGeometry( poFeature->GetGeometryRef() ) )
            && (m_poAttrQuery == NULL
                || m_poAttrQuery->Evaluate( poFeature )) )
            return poFeature;
        delete poFeature;
	}
	return NULL;
}
