#include "header.h"


Object::Object(string filename, Vector3 pos_, Shader shader_){ 
	pos = pos_;
	
	shader = shader_;

	ifstream infile;
	infile.open(filename, ios::in);
	string data;

	int lines = std::count(istreambuf_iterator<char>(infile), istreambuf_iterator<char>(), 'v');

	infile.clear();
	infile.seekg (0, ios::beg);

	lines += std::count(istreambuf_iterator<char>(infile), istreambuf_iterator<char>(), 'f');

	infile.clear();
	infile.seekg (0, ios::beg); //return the stream to the beginning to read the file

	Vector3 temp_verts[MAX_TRIANGLES * 3]; //hypotherically each triangle can have its own 3 vertecies
	int vrt_i = 1; //next index of the temp_verts array to be used

	//the three ints are together a triangle, which is built using index references to vertecies
	int temp_triangles[3][MAX_TRIANGLES];
	int tri_i = 0; //next index of the temp_triangles array to be used


	// loops through the file and populates temp_triangles and temp_verts
	for(int l = 0; l < lines; l++){
		infile >> data;


		if(data[0] == 'v' || data[0] == 'V'){
			size_t sz; 
			
			string s1;
			string s2;
			string s3;
			infile >> s1;
			infile >> s2;
			infile >> s3;	

			float f1 = stof(s1, &sz);
			float f2 = stof(s2, &sz);
			float f3 = stof(s3, &sz);

			//cout << f1 << endl;
			temp_verts[vrt_i] = Vector3(f1, f2, f3);
			vrt_i++;

			
		}
		if(data[0] == 'f' || data[0] == 'F'){
			size_t sz; 

			string s1;
			string s2;
			string s3;
			infile >> s1;
			infile >> s2;
			infile >> s3;	
			
			int i1 = stoi(s1, &sz);
			int i2 = stoi(s2, &sz);
			int i3 = stoi(s3, &sz);

			//cout << i1 << " " << i2 << " " << i3 << endl;

			temp_triangles[0][tri_i] = i1;
			temp_triangles[1][tri_i] = i2;
			temp_triangles[2][tri_i] = i3;

			tri_i++;

		}
	}	


	/*vrt_i is now used as the count of how many trianlges are in the temp_triangles array
	if it is higher than the maximum allowed triangles, then it is shortened
	*/

	if(tri_i > MAX_TRIANGLES)
		tri_i = MAX_TRIANGLES;


	triangle_count = tri_i;

	//cout << triangle_count << endl;

	for(int i = 0; i < tri_i; i++){
		//creates a new triangle from the temp_verts array using the indecies from the three ints in the temp_triangles array


		//cout << temp_verts[temp_triangles[0][i]].z << " ";
		//cout << temp_verts[temp_triangles[0][i]].z << " ";
		//cout << temp_verts[temp_triangles[0][i]].z << endl;

		Triangle new_triangle(temp_verts[temp_triangles[0][i]], temp_verts[temp_triangles[1][i]], temp_verts[temp_triangles[2][i]], shader);

		triangles[i] = new_triangle; //adds the new triangle to the object's triangle list


	}


}

//returns the closes real intersection otherwise return a dummy nonreal intersection
Intersection Object::Closest_Intersection(Vector3 camera_pos, Vector3 ray){
	//the starting closest is an unreal intersection infinite distance away. (1.0/0.0) is +infinity. The other values are dummy
	Intersection closest = Intersection();


	for(int i = 0; i < triangle_count; i++){
		Intersection cur_intx = triangles[i].Check_Intersection(camera_pos, ray, this->pos);

		//if the current intersection is real and closer then it replaces the previous closest
		if(cur_intx.real  && cur_intx.distance < closest.distance){

			closest = cur_intx; 
		}

	}

	//if none are real then the initial dummy is returned, which has a false reality, which should be checked upon return
	return closest;

}
