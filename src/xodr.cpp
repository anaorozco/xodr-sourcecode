/******************************************************************************
 * $Id:  $
 *
 * Project:   OpenGIS Simple Features for OpenDRIVE format
 * Purpose:   Implements OGRXODRDriver class.
 * Authors:   Ana Maria Orozco  <email>
 *			  Michael Scholz    <email> 
 *            Deutsches Zentrum für Luft- und Raumfahrt, DLR
 ******************************************************************************/

#include "xodr.h"
#include "ogr_geometry.h"
#include <cmath>
#include <iostream>
#include <memory>   
#include <string>
#include <typeinfo>
#include <vector>
#include "cpl_conv.h"
#include "OpenDRIVE_1.4E-DRAFT.hxx"

using namespace std;
using namespace xml_schema;

/************************************************************************/
/*                            XODR Class Constructor                    */
/************************************************************************/
XODR::XODR(const char * pszFilename)
{
	pszName = CPLStrdup( pszFilename );
	
	op = (OpenDRIVE_ (pszName, flags::keep_dom | flags::dont_validate));	

	printf ("test reading xml at xodr, getMinorRevision from xodr header:  %d \n ", this->getMinorRevision() );
	
		
}

XODR::~XODR()
{
	// TODO:
	// clean memory
}

/************************************************************************/
/*                       getMinorRevision                               */
/************************************************************************/
int XODR::getMinorRevision(){

	  OpenDRIVE::header_type h;
	  h = op->header(); 
	  header::revMinor_optional& minorRev = h.revMinor();
	  int minorRevision = minorRev.get();

	return minorRevision;
}

/************************************************************************/
/*                      getGeoreference                                 */
/************************************************************************/
const char* XODR::getGeoreference(){
	
	OpenDRIVE::header_type h;
	h = op->header(); 
	header::geoReference_optional& geoRef = h.geoReference();
	std::string geoRefVal = geoRef.get();	
	geoReference = geoRefVal;
	return geoRefVal.c_str();
}

/************************************************************************/
/*                      getGeoreference                                 */
/************************************************************************/
std::string XODR::getGeoReferenceString(){
	
	getGeoreference(); // set GeoReference Value
	return geoReference;
}
/************************************************************************/
/*                     getFeatMultiLineString                           */
/************************************************************************/
 
std::vector<OGRMultiLineString*> XODR::getFeatMultiLineString(){
		 
	OpenDRIVE::road_sequence& rs  (op->road ()); 
	std::vector<OGRMultiLineString*> features_vector;
	std::string ls, ss, as, ps, pps;
	OGRMultiLineString * road_i;

	for (OpenDRIVE::road_iterator i (rs.begin ()); i != rs.end (); ++i)
	   {
		   road& r (*i);
		   road_i = new OGRMultiLineString();
		   road_i -> empty();
		   planView& p = r.planView(); 
		   planView::geometry_sequence& gs (p.geometry());     
		  
		   for(planView::geometry_iterator it (gs.begin ()); it != gs.end (); ++it )
		   {
			   
			   geometry &geometryRef (*it);
			   geometry::line_optional &line_opt (it->line());
			   geometry::spiral_optional &spiral_opt (it->spiral());	
			   geometry::arc_optional &arc_opt (it->arc());	
			   geometry::poly3_optional &poly3_opt (it->poly3());
			   geometry::paramPoly3_optional &paramPoly3_opt (it->paramPoly3());

			   OGRLineString *lineString ;
			   //OGRLineString lineString; 

			   if( line_opt != NULL){
				   ls = typeid(*line_opt).name();
				   if (ls == "class line2")
				   { 											
					   lineString = new OGRLineString( lineToLinestring(geometryRef));					
					   //poLineString = lineString
					   road_i->addGeometry(lineString); 
				   }
			   }

			   if( arc_opt != NULL){	
				   as = typeid(*arc_opt).name();		
				   if (as == "class arc")
				   { 
					   lineString = new OGRLineString( arcToLinestring(geometryRef) );	
					   road_i->addGeometry(lineString);					  
				   }
			   }

			   if( spiral_opt != NULL){
				   ss = typeid(*spiral_opt).name();		
				   if (ss == "class spiral")
				   { 
					   // TODO: convert spirals -> lineString obj
					   // road_i->addGeometry(&lineString);	
				   }
			   }
			   
			   if( poly3_opt != NULL){
				   ps = typeid(*poly3_opt).name();		
				   if (ps == "class poly3")
				   {
					   // TODO: convert poly3 -> lineString obj
					   // road_i->addGeometry(&lineString);	
				   }
			   }
			   
			   if( paramPoly3_opt != NULL){			
				   pps = typeid(*paramPoly3_opt).name();		
				   if (ps == "class paramPoly3")
				   {
					   // TODO: convert paramPoly -> lineString obj
					   // road_i->addGeometry(&lineString);	
				   }
			   }
		   
		   }// end_for_geom

		     //road_i
		   features_vector.push_back(road_i);  // contains obj from Multilinestring
	
	}  // end_for_road
	numRoads = features_vector.size();
	return features_vector;
}

/************************************************************************/
/*                          lineToLinestring                            */
/************************************************************************/
OGRLineString XODR::lineToLinestring(geometry &geoParam){
	// Arguments: Geometry and Line with the following values/[units]
	// s [m], x [m], y [m], hdg [rad], length [m] 
	
	geometry::s_optional& s = geoParam.s();
	geometry::x_optional& x = geoParam.x();
	geometry::y_optional& y = geoParam.y();
	geometry::hdg_optional& hdg = geoParam.hdg();  
	geometry::length_optional& length = geoParam.length();  

	double s_value = s.get();  // reference
	double x_value = x.get();  // initial coordinate x (X_init, Y_init)
	double y_value = y.get();  // initial coordinate y
	double hdg_value = hdg.get();		// heading (polar coordinate)
	double length_value = length.get(); // length
	
	// Calculation of end coordinates (X_end, Y_end), from polar coord and lenght:
	// Formula ( x + cos(hdg) * length, y + sin(hdg) * length)

	double x_end = x_value + (std::cos(hdg_value))*length_value;
	double y_end = y_value + (std::sin(hdg_value))*length_value;

	// Create the OGRLineString and set the (Xi, Yi) coordinates 
	
	OGRLineString lineString = OGRLineString();
	
	// Points set: initial
	OGRPoint po_i (x_value, y_value);
	lineString.addPoint(&po_i);
	lineString.StartPoint(&po_i);
	
	// Points set: end
	OGRPoint po_e (x_end, y_end);
	lineString.addPoint(&po_e);
	lineString.EndPoint(&po_e);
	
	return lineString; 
}

/************************************************************************/
/*                          arcToLinestring                             */
/************************************************************************/
OGRLineString XODR::arcToLinestring(geometry &geoParam){

	// Arguments: Geometry and Arc with the following values/[units]
	// s [m], x [m], y [m], hdg [rad], length [m], curvature [1/m] 
	
	//From: <geometry> tag
	geometry::s_optional& s = geoParam.s();
	geometry::x_optional& x = geoParam.x();
	geometry::y_optional& y = geoParam.y();
	geometry::hdg_optional& hdg = geoParam.hdg();  
	geometry::length_optional& length = geoParam.length();  
	
	// From: <arc> tag
	geometry::arc_optional& arc = geoParam.arc();
	arc::curvature_optional& curv = arc->curvature();
	
	double s_value = s.get();  // reference
	double x_value = x.get();  // initial coordinate x (X_init, Y_init)
	double y_value = y.get();  // initial coordinate y
	double heading = hdg.get();		// heading (polar coordinate)
	double length_value = length.get(); // length	
	double curvature = curv.get();
	
	/* Verification of variables (for testing)
	printf("curvature [1/m]    :  %f \n ", curvature);
	printf("length    [m]      :  %f \n ", length_value);
	printf("heading   [rad ]   :  %f \n ", heading);
	*/	

	// *********     Step 1 : Arc definition     ************
	// To find the points that define the arc (Xo,Yo), (Xm,Ym) and (Xe,Ye)
	// General formula for finding points
	// for polar coordinates with center (0,0)

	double r = 1 / abs(curvature);	
	double alpha = length_value/r;

	double alpha_0 = 0;
	double xo = 0;
	double yo = 0;			
	double xm = 0;
	double ym = 0;
	double xe = 0;
	double ye = 0;

	//   For negative curvature, the points are given by
	//   x = r*sin  and  y = r*-cos 
	if(curvature > 0){

		// Initial point: alpha = 0
		alpha_0 = 0;
		xo = r*sin(alpha_0);
		yo = r*(-cos(alpha_0));
		
		// Middle point: angle = alpha/2
		xm = r*sin(alpha/2);
		ym = r*(-cos(alpha/2));

		// End point: angle = alpha
		xe = r*sin(alpha);
		ye = r*(-cos(alpha));

	} else{
		//  [!] For negative curvature, the points are given by
		//   x = r*sin  and  y = r*cos 

		// Initial point: alpha = 0
		alpha_0 = 0;
		xo = r*sin(alpha_0);
		yo = r*(cos(alpha_0));
		
		// Middle point: angle = alpha/2
		xm = r*sin(alpha/2);
		ym = r*(cos(alpha/2));

		// End point: angle = alpha
		xe = r*sin(alpha);
		ye = r*(cos(alpha));  
	}

	// *********  Step 2 : Arc Translation ************
	// Tranlation of the points with the Translation matrix T 
	// General formula for finding the points 
	// XY_T = T + P    T = [0, r]
	
	// Variables initialization 
	double xt = 0;
	double yt = 0;
	
	// Translation matrix definition for positive curvature
	if (curvature > 0){
		xt = 0;
		yt = r;
	}else{
		xt = 0;
		yt = -r;
	}

	// Translation: Initial point
	double xo_t = xt + xo; 
	double yo_t = yt + yo;
	
	// Translation: Middle point
	double xm_t = xt + xm; 
	double ym_t = yt + ym;
	
	// Translation: End point
	double xe_t = xt + xe; 
	double ye_t = yt + ye;


	/* *********  Step 3 : Arc Rotation  ************
	// Rotation of the points with the heading
	// General formula for finding the points 
	// R =  cos  -sin
	//      sin   cos								 */
		
	// Rotation: Initial point
	xo = xo_t*cos(heading) - yo_t*sin(heading);
	yo = xo_t*sin(heading) + yo_t*cos(heading);
	
	// Rotation: Middle point
	xm = xm_t*cos(heading) - ym_t*sin(heading);
	ym = xm_t*sin(heading) + ym_t*cos(heading);
	
	// Rotation: End point
	xe = xe_t*cos(heading) - ye_t*sin(heading);
	ye = xe_t*sin(heading) + ye_t*cos(heading);
	
	
	/* *********  Step 4 : Translation from polar to OpenDRIVE coordinates *******
	// From origen (0,0) of the polar coordinates to the Openroad (x,y)  
	// coordinates plane. 
	// coord_final (x_f, y_f) = polar + opendrive								*/
	
	// Translation: Initial point
	double xo_f = xo + x_value;
	double yo_f = xo + y_value;
	
	// Translation: Middle point
	double xm_f = xm + x_value;
	double ym_f = ym + y_value;

	// Translation: End point
	double xe_f = xe + x_value;
	double ye_f = ye + y_value;


	// *********  Step 5 : Create the geometry as OGRLineString *******
	// Sample rate, given by sampling angle: 15 grades
	// Initial, middle and end points with translation (r), rotation(hdg) and plane translation
	
	OGRLineString* arcLineString = OGRGeometryFactory::curveToLineString(xo_f, yo_f, 0, xm_f, ym_f, 0, xe_f, ye_f, 0, false, 15, NULL);	
	return (*arcLineString);
}

/************************************************************************/
/*                       getNumberOfRoads()                             */
/************************************************************************/
int XODR::getNumberOfRoads(){
	OpenDRIVE::road_sequence& rs  (op->road ());
	for (OpenDRIVE::road_iterator i (rs.begin ()); i != rs.end (); ++i)
	   {
		   road& r (*i);
		   planView& p = r.planView(); 
		   planView::geometry_sequence& gs (p.geometry());     
		   for(planView::geometry_iterator it (gs.begin ()); it != gs.end (); ++it )
		   {
			   numRoads++;
		   };
		}

	return numRoads;
}

/************************************************************************/
/*                       getNumberOfSignals()                             */
/************************************************************************/
int XODR::getNumberOfSignals(){
	
	return numSignals;
}

	
