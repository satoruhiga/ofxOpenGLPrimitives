

--- vertex

in vec3 position;

in vec3 instance_position;
in vec4 instance_color;

out vec4 vs_color;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

uniform float time;

mat4 MVP = projection_matrix * view_matrix * model_matrix;

mat4 rotationMatrix(vec3 axis, float angle)
{
	axis = normalize(axis);
	float s = sin(angle);
	float c = cos(angle);
	float oc = 1.0 - c;
	
	return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
				oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
				oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
				0.0,                                0.0,                                0.0,                                1.0);
}

void main(){
	mat4 R = rotationMatrix(instance_position, time);
	gl_Position = MVP * vec4((R * vec4(position, 1)).xyz + instance_position, 1);
	vs_color = instance_color;
}

--- fragment

in vec4 vs_color;

out vec4 frag_color;

void main(){
    frag_color = vs_color;
}
