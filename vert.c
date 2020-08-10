attribute vec4 pos;
varying mediump vec2 vCoord;

uniform mat4 projection;
uniform mat4 mv;
uniform mediump float twist;
uniform mediump float turn;

const mediump float PI = 3.1415926535897932384626;

void main(void) {
	// hacky unpacky things that should really have been separate attributes and uniforms
	mediump float side = sign(pos.z);
	mediump float ribbon_width = pos.w;
	mediump float ribbon_extension = 1.0 + 10000.0 * step(2.0, abs(pos.z));

	// coordinates to pass downstream to texturing
	vCoord = 0.25 + 0.25 * pos.xy;
	vCoord.y = 0.5 + (vCoord.y - 0.5) * side;

	mediump vec3 xyz = vec3(pos.xy, side);

	// twist 'em up!
	xyz.y += xyz.z * twist * (xyz.x + 1.0);
	mediump float flip = 1.0 - step(xyz.y * xyz.z, 1.0);
	xyz.y = mix(xyz.y, xyz.z * (2.0 - xyz.y * xyz.z), flip);
	xyz.z *= 1.0 - 2.0 * flip;

	// turn 'em 'round!
	mediump vec2 cis_turn = vec2(cos(turn), sin(turn));
	mediump mat2 mat_turn = mat2(cis_turn, cis_turn.yx);
	mat_turn[1][0] *= -1.0;

	// convert to trig scale
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

	// ends off to infinity and their attachments
	mediump vec2 minor_axis = mix(vec2(0.0, 1.0), vec2(-twist_major_axis.y, twist_major_axis.x), 0.5 + 0.5 * xyz.x);
	xyz.yz += ribbon_width * minor_axis;
	xyz *= ribbon_extension;

	// cosmetic: pointy pillow shape
	mediump float scale = max(0.0, dot(xyz, xyz) - 1.0);
	scale = 1.0 + scale * scale;
	scale = (1.0 + 0.2 * scale) / 1.2;

	xyz.xy = mat_turn * xyz.xy;
	xyz *= scale;
	gl_Position = projection * mv * vec4(xyz, 1.0);
}
