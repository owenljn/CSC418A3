/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements light_source.h

***********************************************************/

#include <cmath>
#include "light_source.h"

void PointLight::shade( Ray3D& ray ) {
	// TODO: implement this function to fill in values for ray.col 
	// using phong shading.  Make sure your vectors are normalized, and
	// clamp colour values to 1.0.
	//
	// It is assumed at this point that the intersection information in ray 
	// is available.  So be sure that traverseScene() is called on the ray 
	// before this function.  
	
	// Initialize s and n
	Vector3D s = _pos - ray.intersection.point;
	Vector3D n = ray.intersection.normal;
	// Normalize s and n
	s.normalize();
	n.normalize();
	
	// Initialize m and d
	Vector3D m = 2*n.dot(s)*n - s;
	Vector3D d = -ray.dir;
	// Normalize m and d
	m.normalize();
	d.normalize();

	// Initialize Ia Id Is
	Colour Ia = _col_ambient;
	Colour Id = _col_diffuse;
	Colour Is = _col_specular;
	Colour Ra = ray.intersection.mat->ambient;
	Colour Rd = ray.intersection.mat->diffuse;
	Colour Rs = ray.intersection.mat->specular;
	
	// Initialize alpha
	double alpha = ray.intersection.mat->specular_exp;

	// Three different light reflections
	Colour diffuseReflec = fmax(0, s.dot(n))*Rd*Id;
	Colour ambientReflec = Ra*Ia;
	Colour specularReflec = pow(fmax(0, m.dot(d)), alpha)*Rs*Is;

	// Please comment/uncomment the following lines of code to see
	// different rendering styles

	// This only generates the scene signiture
	ray.col = ray.col + Rd;
	ray.col.clamp();

	// This generates the scene signiture for Phong model with only
	// diffuse and ambient
	//ray.col = ray.col + diffuseReflec + ambientReflec;
	//ray.col.clamp();

	// This generates the scene signiture for Phong model
	//ray.col = ray.col + diffuseReflec + ambientReflec + specularReflec;
	//ray.col.clamp();
}

