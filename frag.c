varying mediump vec2 vCoord;
varying mediump vec3 vPosition;
varying mediump vec3 vNormal;

uniform highp mat4 mv;  // highp is needed to access the same uniform as vert.c
uniform mediump mat2 textureTransform;  // this is the inverse of what the picture looks like
// (0.5 Z)**2 semidirect SL(2,Z) acts on the dumpling with kernel Z**2;
// textureTransform is the SL(2,Z) part and half_offset is the ((0.5 Z) / Z)**2 part.
uniform mediump vec2 half_offset;
uniform mediump vec3 light_from;  // points at the light

void main(void) {
	// vCoord = coordinates on dumpling surface
	// front = [0, 0.5] x [0, 0.5], back = [0, 0.5] x [0.5, 1]
	mediump vec2 texCoord = mod(textureTransform * (vCoord.xy + 0.5 * half_offset), 1.0);
	texCoord = mix(texCoord, 1.0 - texCoord, step(0.5, texCoord.x));

	// stringness = 1 in the middle of the string/ribbon, 0 at its edges
	mediump float stringness = abs(1.0 - 2.0 * mod(2.0 * texCoord.y, 1.0));
	mediump float string_thickness = 16.0 * texCoord.x * (0.5 - texCoord.x);
	string_thickness = max(0.5 * sqrt(string_thickness), 1e-3);
	if (stringness < 1.0 - string_thickness) { discard; }

	// nice color gradients for the ribbons
	mediump vec2 color_coord = 2.0 * texCoord;
	color_coord.y = min(color_coord.y, 2.0 - color_coord.y);
	mediump vec3 color = mix(
		vec3(0.0, 0.7 - 0.6 * color_coord.x, 0.1 + 0.7 * color_coord.x),
		vec3(1.0, 0.1 + 0.5 * color_coord.x, 0.0),
		step(0.5, color_coord.y)
	);

	// diffuse ("litness") and specular ("highlight") lighting
	mediump vec3 normal = normalize(vNormal);
	mediump float litness = dot(normal, light_from);
	mediump vec3 moved_normal = (mv * vec4(normal, 0.0)).xyz;
	mediump float visible_side = 1.0 - 2.0 * step(0.0, dot(moved_normal, vPosition));
	mediump vec3 apparent_normal = visible_side * moved_normal;  // flip normals for backface
	mediump float highlight = max(0.0,
		dot(reflect(normalize(vPosition), apparent_normal), (mv * vec4(light_from, 0.0)).xyz));
	highlight = highlight * highlight;
	highlight = highlight * highlight;
	highlight = highlight * highlight;

	// slightly dim backface
	color *= (0.2 + (step(0.0, litness) * 0.2 + 0.6) * abs(litness));
	color = mix(color, vec3(1.0), highlight * (0.5 + 0.5 * step(0.0, visible_side)));

	// convert linear to srgb
	color = mix(12.92 * color, 1.055 * pow(color, vec3(1.0/2.4)) - vec3(0.055), step(0.0031308, color));

	gl_FragColor = vec4(color, 1.0);
}
