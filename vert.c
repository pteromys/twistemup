attribute vec4 pos;
varying mediump vec2 vCoord;

uniform mat4 projection;
uniform mat4 mv;
uniform mediump float twist;
uniform mediump float turn;

const mediump float PI = 3.1415926535897932384626;

void main(void) {
	// coordinates to pass downstream to texturing
	vCoord = 0.25 + 0.25 * pos.xy;
	vCoord.y = 0.5 + (vCoord.y - 0.5) * pos.w;

	mediump vec3 xyz = pos.xyw;

	// twist 'em up!
	xyz.y += xyz.z * twist * (xyz.x + 1.0);
	mediump float flip = 1.0 - step(xyz.y * xyz.z, 1.0);
	xyz.y = mix(xyz.y, xyz.z * (2.0 - xyz.y * xyz.z), flip);
	xyz.z *= 1.0 - 2.0 * flip;

	// turn 'em 'round!
	mediump vec2 cis_turn = vec2(cos(turn), sin(turn));
	mediump mat2 mat_turn = mat2(cis_turn, cis_turn.yx);
	mat_turn[1][0] *= -1.0;

	// linear vs trigonometric scale
	// mediump vec2 xy = xyz.xy;
	xyz.xy = -cos((0.5 + 0.5 * xyz.xy) * PI);

	// bulging pillow shape with twist
	xyz.z *= sqrt(1.0 - xyz.y * xyz.y);
	mediump vec2 twist_major_axis = vec2(cos(twist * PI), -sin(twist * PI));
	mediump vec4 twist_quadratic_form = vec4(
		twist_major_axis.x * twist_major_axis,
		twist_major_axis.y * twist_major_axis
	);
	twist_quadratic_form = mix(vec4(1.0, 0.0, 0.0, 0.0), twist_quadratic_form, 0.5 + 0.5 * xyz.x);
	twist_quadratic_form = mix(twist_quadratic_form, vec4(1.0, 0.0, 0.0, 1.0), sqrt(1.0 - xyz.x * xyz.x));
	xyz.yz = mat2(twist_quadratic_form.xy, twist_quadratic_form.zw) * xyz.yz;

	// cosmetic: pointy pillow shape
	mediump float scale = max(0.0, dot(xyz.xy, xyz.xy) - 1.0);
	scale = 1.0 + scale * scale;
	scale = (1.0 + 0.2 * scale) / 1.2;

	xyz = vec3(
		mat_turn * xyz.xy * scale,
		xyz.z * 0.6
	);
	// xyz.yz = mat_twist * xyz.yz;
	gl_Position = projection * mv * vec4(xyz, 1.0);
}
