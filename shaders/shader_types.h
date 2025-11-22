/*
	File: shader_types.h
	Description: Shared type definitions for shader rendering
*/

#ifndef SHADER_TYPES_H
#define SHADER_TYPES_H

// Core structures for shader rendering
struct FramebufferState
{
	uint32_t framebuffer = 0, renderTexture = 0, rbo = 0;
	int last_display_w = 0, last_display_h = 0;
	bool initialized = false;
};

struct ShaderQuad
{
	uint32_t VAO = 0, VBO = 0;
	void initialize();
	void cleanup();
};

// burn in accumulation buffer
struct AccumulationBuffers
{
	FramebufferState accum[2];
	bool swap = false;
	// Note: burnInShader is now managed by ShaderManager
};

#endif // SHADER_TYPES_H