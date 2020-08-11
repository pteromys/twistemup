varying mediump vec2 vCoord;
varying mediump vec3 vPosition;
varying mediump vec3 vNormal;

uniform highp mat4 mv;  // highp is needed to access the same uniform as vert.c
uniform mediump mat2 textureTransform;
uniform mediump vec2 half_offset;
uniform mediump vec3 light_from;

void main(void) {
	mediump vec2 texCoord = mod(textureTransform * (vCoord.xy + 0.5 * half_offset), 1.0);
	texCoord = mix(texCoord, 1.0 - texCoord, step(0.5, texCoord.x));
	mediump float stringness = abs(1.0 - 2.0 * mod(2.0 * texCoord.y, 1.0));
	mediump float string_thickness = 16.0 * texCoord.x * (0.5 - texCoord.x);
	string_thickness = max(0.5 * sqrt(string_thickness), 1e-3);
	if (stringness < 1.0 - string_thickness) { discard; }

	mediump vec2 color_coord = 2.0 * texCoord;
	color_coord.y = min(color_coord.y, 2.0 - color_coord.y);
	mediump vec3 color = mix(
		vec3(0.0, 0.8 - 0.4 * color_coord.x, 0.4 + 0.4 * color_coord.x),
		vec3(1.0, 0.4 + 0.4 * color_coord.x, 0.0),
		step(0.5, color_coord.y)
	);
	mediump vec3 normal = normalize(vNormal);
	mediump float litness = dot(normal, light_from);
	mediump vec3 moved_normal = (mv * vec4(normal, 0.0)).xyz;
	mediump float visible_side = 1.0 - 2.0 * step(0.0, dot(moved_normal, vPosition));
	mediump vec3 apparent_normal = visible_side * moved_normal;
	mediump float highlight = max(0.0,
		dot(reflect(normalize(vPosition), apparent_normal), (mv * vec4(light_from, 0.0)).xyz));
	highlight = highlight * highlight;
	highlight = highlight * highlight;
	highlight = highlight * highlight;

	color *= (0.5 + (step(0.0, litness) * 0.2 + 0.3) * abs(litness));
	color = mix(color, vec3(1.0), highlight * (0.5 + 0.5 * step(0.0, visible_side)));
	gl_FragColor = vec4(color, 1.0);
}
