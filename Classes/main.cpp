#include "header.h"
#include "bitmap.c"

const float PI = 3.1415926;

const int MAX_BOUNCE = 3;
const float DIFFUSE_RAY_DENSITY = 100;
const float DIFFUSE_RAY_SPREAD = (4*PI)/5;

const int width = 200;
const int height = 200;

Intersection Cast(Vector3 ray, Vector3 camera_pos, Object objects[], int object_count, int max_bounce, int bounce){
	
	Vector3 sky_color = Vector3(64, 64, 64);
	Intersection intx = Intersection(sky_color);

	//Cast ray to each object. Overwrite inx if the new intersection is closer
	for(int o = 0; o < object_count; o++){

		Intersection new_intx = objects[o].Closest_Intersection(camera_pos, ray);
		if(new_intx.distance < intx.distance){ intx = new_intx;}
	}

	//-1 bounce means that we don't care about the color, we just want the other intersection data
	if(bounce == -1){
		return intx;
	}

	Shader shader = intx.shader;
	if(intx.real){
		
		
		bool shaded = Cast(Vector3(-1, 0, 1), intx.point, objects, object_count, max_bounce, -1).real;
		Vector3 flat_color;
		if(!shaded)
			flat_color = shader.color;
		else{
			float r = (sky_color.x*shader.color.x)/255.0;
			float g = (sky_color.y*shader.color.y)/255.0;
			float b = (sky_color.z*shader.color.z)/255.0;

			flat_color = Vector3(r, g, b);
		}

		Vector3 reflective_color = Vector3(0, 0, 0);
		if(bounce < max_bounce && shader.reflective > 0){
			//Formula for a reflection ray
			//https://math.stackexchange.com/questions/13261/how-to-get-a-reflection-vector
			Vector3 normal = intx.normal;
			Vector3 reflection_ray = (ray - (normal * ray.Dot(normal) * 2.0)).Adjusted_Magnitude(1);
			
			reflective_color = Cast(reflection_ray, intx.point, objects, object_count, max_bounce, bounce + 1).color;
			
			reflective_color.x = (shader.color.x/255) * reflective_color.x;
			reflective_color.y = (shader.color.y/255) * reflective_color.y;
			reflective_color.z = (shader.color.z/255) * reflective_color.z;
		}

		Vector3 refractive_color = Vector3(0, 0, 0);
		if(bounce < max_bounce && shader.refractive > 0){
			Vector3 normal = intx.normal;
			
			Vector3 refraction_ray = Vector3(1, ray.Angle() + (ray.Angle()-normal.Angle()) );
			
			refractive_color = Cast(refraction_ray, intx.point + refraction_ray, objects, object_count, max_bounce, bounce).color;
			
			refractive_color.x = (shader.color.x/255) * refractive_color.x;
			refractive_color.y = (shader.color.y/255) * refractive_color.y;
			refractive_color.z = (shader.color.z/255) * refractive_color.z;
		}
		
		Vector3 diffuse_color = Vector3(0, 0, 0);
		if(bounce < max_bounce && shader.diffuse > 0){
			
			
			for(int i = 0; i < DIFFUSE_RAY_DENSITY; i++){
				
				
				Vector3 normal = Vector3(intx.normal.x, intx.normal.y, intx.normal.z);
				if(ray.Dot(intx.normal) > 0){
					normal = normal * -1;
				}

				//Get a random vector, bur change it so it can only point away from the plane
				//Vector3 randomVector = Vector3(0.99, Vector3(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX)) + normal;
				//randomVector.Set_Magnitude(1);
				
				Vector3 angle = normal.Angle();
				float d_angle_x =  DIFFUSE_RAY_SPREAD * (( rand()/(float)RAND_MAX) - 0.5);
				float d_angle_y =  DIFFUSE_RAY_SPREAD * (( rand()/(float)RAND_MAX) - 0.5);
				Vector3 random = Vector3(1, Vector3(angle.x+d_angle_x, angle.y+d_angle_y));

				diffuse_color = diffuse_color + Cast(random, intx.point, objects, object_count, max_bounce, bounce + 1).color;
			}


			//Find the average color
			diffuse_color = diffuse_color * (1.0f / DIFFUSE_RAY_DENSITY);

			//Dampen the reflection color based on the color of the object
			diffuse_color.x = (shader.color.x/255) * diffuse_color.x;
			diffuse_color.y = (shader.color.y/255) * diffuse_color.y;
			diffuse_color.z = (shader.color.z/255) * diffuse_color.z;
		}

		intx.color = reflective_color*shader.reflective +
		             flat_color*shader.flat +
					 diffuse_color*shader.diffuse +
					 refractive_color*shader.refractive;


		return intx;

	}
	
	return intx;


}


Vector3 camera_pos = Vector3(0, 0, 0);
float FOV = (70*PI)/180;
Vector3 camera_angle(0, -(PI/8), 0);
unsigned char pixels[width * height * 3]; 

int main(){
	
	
	Vector3 empty(0, 0, 0);

	int object_count = 4;
	Object objects[4];	
	objects[0] = Object("objects\\floor.obj", Vector3(0, 0, 0), 
		Shader{0.5, 0.5, 0, 0, 1, Vector3(200, 200, 200)} );
	objects[1] = Object("objects\\cube.obj", Vector3(-1, 0, 0.1), 
		Shader{0, 0, 0, 1, 1, Vector3(100, 100, 255)} );
	objects[2] = Object("objects\\cube.obj", Vector3(-2, -2.5, 0.1),
		Shader{0.5, 0.5, 0, 0, 1, Vector3(0, 255, 0)});
	objects[3] = Object("objects\\sphere2.obj", Vector3(2, -0, 0.1), 
		Shader{0.8, 0.2, 0, 0, 1, Vector3(255, 0, 0)});
	

 	for(int z = 0; z <= 40; z ++) //z is the number of steps in the multi-frame animation
	{
		float theta = z/20.0 * PI;
		Vector3 camera_angle = Vector3(-theta, 0); 
		Vector3 camera_pos = Vector3(-15.0 * cos(theta), 15.0 * sin(theta), 0.5);



		//Vectors pointing to the corners of the viewport 
		Vector3 viewport_left = Vector3(-tan(FOV/2.0), Vector3(camera_angle.x + (PI/2.0), 0, 0))+Vector3(1, camera_angle);
		Vector3 viewport_right = Vector3(tan(FOV/2.0), Vector3(camera_angle.x + (PI/2.0), 0, 0))+Vector3(1, camera_angle);
		
		Vector3 viewport_top = Vector3(-tan(FOV/2.0), Vector3(0, camera_angle.y + (PI/2.0), 0))+Vector3(1, camera_angle);
		Vector3 viewport_bottom = Vector3(tan(FOV/2.0), Vector3(0, camera_angle.y + (PI/2.0), 0))+Vector3(1, camera_angle);

		for(int y = 0; y < height; y++){
			cout << y << endl;

			
			for(int x = 0; x < width; x++){
				//cout << "\t" << x << endl;
				

				float local_y_pos = ( (float)y/height);
				float local_x_pos = ( (float)x/width);

				//Interpolate between the left and the right
				Vector3 top_x = (viewport_left + viewport_top)*(1.0-local_x_pos) + (viewport_right + viewport_top)*local_x_pos;
				Vector3 bottom_x = (viewport_left + viewport_bottom)*(1.0-local_x_pos) + (viewport_right + viewport_bottom)*local_x_pos;

				//interpolate between the top and the bottom
				Vector3 ray = top_x*(1-local_y_pos) + bottom_x*local_y_pos;
				ray.Set_Magnitude(1);

				Vector3 color = Cast(ray, camera_pos, objects, object_count, MAX_BOUNCE, 0).color;

				pixels[(y*width*3) + (x*3)] = (unsigned char) (color.z);
				pixels[(y*width*3) + (x*3) + 1] = (unsigned char) (color.y);
				pixels[(y*width*3) + (x*3) + 2] = (unsigned char) (color.x);

				

			}

			//cout << y << endl;
			

		}

		string saveDirS = "output\\" + to_string(z) + ".bmp";
		int stringLen = saveDirS.length();
		
		//Maximum length for a filename is 99 chars. TODO: Fix this later
		char saveDir[100];
		saveDir[stringLen] = '\0';
		for(int i = 0; i < stringLen; i++){
			saveDir[i] = saveDirS[i];
		}
		cout << saveDir << endl;


		Save(pixels, width, height, saveDir);
	}


	return 0;
}