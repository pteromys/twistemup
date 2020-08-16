attribute vec4 pos;
varying mediump vec2 vCoord;
varying mediump vec3 vPosition;
varying mediump vec3 vNormal;

uniform mat4 projection;
uniform mat4 mv;
uniform mediump float twist;
uniform mediump float turn;

const mediump float PI = 3.1415926535897932384626;
const mediump float EPSILON = 1e-6;  // margin added to all denominators

mat2 mat2_from_vec4(vec4 v) {
	return mat2(v.xy, v.zw);
}

void main(void) {
	// hacky unpacky things that should really have been separate attributes and uniforms
	mediump float side = sign(pos.z);
	mediump float ribbon_width = pos.w;
	mediump float ribbon_extension = 1.0 + 1.0 * step(2.0, abs(pos.z));

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

	// convert input coordinates to trigonometric scale
	// for higher resolution at the edges of the square where z will change faster
	xyz.xy = -cos((0.5 + 0.5 * xyz.xy) * PI);

	// bulging dumpling shape with twist
	// the yz cross-section at x is (mat2 ellipse) * (the unit circle).
	xyz.z *= sqrt(1.0 - xyz.y * xyz.y);
	mediump vec2 twist_major_axis = vec2(cos(twist * PI), -sin(twist * PI));
	mediump vec4 twist_end_projector = vec4(
		twist_major_axis.x * twist_major_axis,
		twist_major_axis.y * twist_major_axis
	);
	mediump float twist_weight = 0.5 + 0.5 * xyz.x;
	mediump float inflate_weight = sqrt(1.0 - xyz.x * xyz.x);
	mediump mat2 ellipse = mat2_from_vec4(mix(
		mix(vec4(1.0, 0.0, 0.0, 0.0), twist_end_projector, 0.5 + 0.5 * xyz.x),
		vec4(1.0, 0.0, 0.0, 1.0),
		inflate_weight
	));
	mediump vec2 circle_preimage = xyz.yz;  // used below for normals
	xyz.yz = ellipse * xyz.yz;

	// compute normal at this stage as the gradient of length(inverse(ellipse) * xyz.yz)**2
	inflate_weight += EPSILON;
	mediump mat2 dx_ellipse = mat2_from_vec4(
		-vec4(1.0, 0.0, 0.0, 1.0) * xyz.x
		+ 0.5 * twist_end_projector * (xyz.x + 2.0 * xyz.x * xyz.x - 1.0 + inflate_weight)
		+ 0.5 * vec4(1.0, 0.0, 0.0, 0.0) * (xyz.x - 2.0 * xyz.x * xyz.x + 1.0 - inflate_weight)
	);  // missing a factor of 1/inflate_weight so we correct it below in defining normal
	mediump mat2 adjugate_ellipse = mat2(
		ellipse[1][1], -ellipse[0][1],
		-ellipse[1][0], ellipse[0][0]
	);
	mediump vec3 normal = vec3(
		dot(-adjugate_ellipse * dx_ellipse * circle_preimage, circle_preimage),
		inflate_weight * adjugate_ellipse * circle_preimage
	);
	normal = mix(vec3(xyz.x, 0.0, 0.0), normal, step(EPSILON, dot(normal, normal)));

	// ends off to infinity and their attachments
	normal = mix(vec3(-xyz.x, xyz.yz), normal, step(abs(ribbon_width), 0.0));
	mediump vec2 minor_axis = mix(vec2(0.0, 1.0), vec2(-twist_major_axis.y, twist_major_axis.x), 0.5 + 0.5 * xyz.x);
	xyz.yz += ribbon_width * minor_axis;
	xyz *= ribbon_extension;

	// cosmetic: pointy dumpling shape
	mediump float scale = max(0.0, dot(xyz, xyz) - 1.0);
	scale = 1.0 + scale * scale;
	scale = (1.0 + 0.2 * scale) / 1.2;

	xyz.xy = mat_turn * xyz.xy;
	normal.xy = mat_turn * normal.xy;
	xyz *= scale;
	highp vec4 apparent_position = mv * vec4(xyz, 1.0);
	gl_Position = projection * apparent_position;
	vPosition = apparent_position.xyz;
	vNormal = normalize(normal);
}
