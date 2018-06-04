/***********************************************************
     Starter code for Assignment 3

     This code was originally written by Jack Wang for
		    CSC418, SPRING 2005

		implements scene_object.h

***********************************************************/

#include <cmath>
#include <iostream>
#include "scene_object.h"

bool UnitSquare::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	// TODO: implement intersection code for UnitSquare, which is
	// defined on the xy-plane, with vertices (0.5, 0.5, 0), 
	// (-0.5, 0.5, 0), (-0.5, -0.5, 0), (0.5, -0.5, 0), and normal
	// (0, 0, 1).
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point, 
	// intersection.normal, intersection.none, intersection.t_value.   
	//
	// HINT: Remember to first transform the ray into object space  
	// to simplify the intersection test.
	
	// Define center of sphere and direction
	Point3D a(0, 0, 0);
	Point3D c = worldToModel * ray.origin;
	Vector3D dir = worldToModel * ray.dir;
	
	// Initialize intersection point
	double lambda = -c[2]/dir[2];
	Point3D intersectP = c + lambda*dir;

	if (ray.intersection.none || ray.intersection.t_value > lambda){
		// return false if no intersection
		if (intersectP[0] < -0.5 || intersectP[0] > 0.5 ||
			intersectP[1] < -0.5 || intersectP[1] > 0.5) {
				return false;
		}
		
		// Initialize normal vector
		Vector3D normal(0, 0, 1);

		// Fill the intersection with correct values
		ray.intersection.point = modelToWorld* (c + lambda*dir);
		ray.intersection.normal = transNorm(worldToModel, normal);
		ray.intersection.normal.normalize();
		ray.intersection.none = false;
		ray.intersection.t_value = lambda;
		return true;
	}
}

bool UnitSphere::intersect( Ray3D& ray, const Matrix4x4& worldToModel,
		const Matrix4x4& modelToWorld ) {
	// TODO: implement intersection code for UnitSphere, which is centred 
	// on the origin.  
	//
	// Your goal here is to fill ray.intersection with correct values
	// should an intersection occur.  This includes intersection.point, 
	// intersection.normal, intersection.none, intersection.t_value.   
	//
	// HINT: Remember to first transform the ray into object space  
	// to simplify the intersection test.

	// Define center of sphere and direction
	Point3D a(0, 0, 0);
	Point3D c = worldToModel * ray.origin;
	Vector3D dir = worldToModel * ray.dir;
	Vector3D vector = c - a;
	
	// Define A, B, C, D and lambda
	double A = dir.dot(dir);
	double B = vector.dot(dir);
	double C = vector.dot(vector) - 1;
	double D = B*B - A*C;
	double lambda;

    // Decide which intersection to choose
	if (D < 0) {
		return false;
	} else if (D == 0) {
		lambda = -B/A;
	} else {
		double lambda1 = -(B/A) + (sqrt(D)/A);
		double lambda2 = -(B/A) - (sqrt(D)/A);
		if (lambda1 < 0 && lambda2 < 0) {
			return false;
		} else if (lambda1 > lambda2 && lambda2 > 0){
			lambda = lambda2;
		} else {
			lambda = lambda1;
		}
	}
	
	// Initialize normal vector
	Vector3D normal = c + lambda*dir - a;
	normal.normalize();
	
	// Fill the intersection with correct values
	ray.intersection.point = modelToWorld* (c + lambda*dir);
	ray.intersection.normal = transNorm(worldToModel, normal);
	ray.intersection.normal.normalize();
	ray.intersection.none = false;
	ray.intersection.t_value = lambda;
	
	return true;
}

