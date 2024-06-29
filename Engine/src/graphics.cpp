#include <gtc\type_ptr.hpp>
#include <gtc\matrix_transform.hpp>

#include "graphics.h"
#include "mesh.h"
#include "texture.h"
#include "program.h"
#include "font.h"
#include "helper.h"

static CMesh* RectangleMesh;

static CProgram* TextureProgram;
static CProgram* SubTextureProgram;
static CProgram* ColouredProgram;

static GLuint TextVAO;
static GLuint TextVBO;
static CProgram* TextProgram;

void GFX::Initialise()
{
	RectangleMesh = CMesh::GetMesh("2D Square");

	TextureProgram = CProgram::GetProgram("Texture");
	SubTextureProgram = CProgram::GetProgram("SubTexture");
	ColouredProgram = CProgram::GetProgram("Colour");

	TextProgram = CProgram::GetProgram("Text");

	// Create text mesh
	glGenVertexArrays(1, &TextVAO);
	glBindVertexArray(TextVAO);

	GLuint ebo = 0;
	GLuint indicies[] = {
		0, 1, 2, 
		0, 2, 3, };

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

	glGenBuffers(1, &TextVBO);
	glBindBuffer(GL_ARRAY_BUFFER, TextVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * 4, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void GFX::DrawImage(GLuint aTexture, glm::mat4& pvm)
{
	// Select program
	glUseProgram(TextureProgram->GetProgramID());

	// Bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, aTexture);
	glUniform1i(TextureProgram->GetUniform("tex"), 0);

	// Set the PVM matrix
	glUniformMatrix4fv(TextureProgram->GetUniform("PVM"), 1, GL_FALSE, glm::value_ptr(pvm));

	// Draw the rectangle
	RectangleMesh->Draw();

	// Unbind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Unbind the program
	glUseProgram(0);
}

void GFX::DrawSubImage(GLuint aTexture, glm::mat4& pvm, glm::ivec4& locSize)
{
	// Select program
	glUseProgram(SubTextureProgram->GetProgramID());

	// Bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, aTexture);
	glUniform1i(SubTextureProgram->GetUniform("tex"), 0);

	// Set the PVM matrix
	glUniformMatrix4fv(SubTextureProgram->GetUniform("PVM"), 1, GL_FALSE, glm::value_ptr(pvm));

	// Set the sub area
	glUniform4i(SubTextureProgram->GetUniform("rect"), locSize.x, locSize.y, locSize.z, locSize.w);

	// Draw the rectangle
	RectangleMesh->Draw();

	// Unbind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Unbind the program
	glUseProgram(0);
}

void GFX::DrawSubImage(GLuint aTexture, glm::mat4& pvm, glm::ivec2& location, glm::ivec2& size)
{
	// Select program
	glUseProgram(SubTextureProgram->GetProgramID());

	// Bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, aTexture);
	glUniform1i(SubTextureProgram->GetUniform("tex"), 0);

	// Set the PVM matrix
	glUniformMatrix4fv(SubTextureProgram->GetUniform("PVM"), 1, GL_FALSE, glm::value_ptr(pvm));

	// Set the sub area
	glUniform4i(SubTextureProgram->GetUniform("rect"), location.x, location.y, size.x, size.y);

	// Draw the rectangle
	RectangleMesh->Draw();

	// Unbind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Unbind the program
	glUseProgram(0);
}

void GFX::DrawSubImage(GLuint aTexture, glm::mat4& pvm, int x, int y, int width, int height)
{
	// Select program
	glUseProgram(SubTextureProgram->GetProgramID());

	// Bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, aTexture);
	glUniform1i(SubTextureProgram->GetUniform("tex"), 0);

	// Set the PVM matrix
	glUniformMatrix4fv(SubTextureProgram->GetUniform("PVM"), 1, GL_FALSE, glm::value_ptr(pvm));

	// Set the sub area
	glUniform4i(SubTextureProgram->GetUniform("rect"), x, y, width, height);

	// Draw the rectangle
	RectangleMesh->Draw();

	// Unbind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Unbind the program
	glUseProgram(0);
}

void GFX::DrawRectangle(glm::mat4& pv, glm::vec4 colour, int x, int y, int width, int height)
{
	glm::mat4 pvm = pv * CalculateModelMatrix(glm::vec2(x, y), glm::vec2(width, height));

	// Select program
	glUseProgram(ColouredProgram->GetProgramID());
	
	// Set the PVM matrix
	glUniformMatrix4fv(ColouredProgram->GetUniform("PVM"), 1, GL_FALSE, glm::value_ptr(pvm));

	// Set the colour
	glUniform4f(ColouredProgram->GetUniform("objColour"), colour.x, colour.y, colour.z, colour.w);

	// Draw the rectangle
	RectangleMesh->Draw();

	// Unbind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Unbind the program
	glUseProgram(0);
}

void GFX::DrawRectangle(glm::mat4& pv, glm::vec3 colour, int x, int y, int width, int height)
{
	glm::mat4 pvm = pv * CalculateModelMatrix(glm::vec2(x, y), glm::vec2(width, height));

	// Select program
	glUseProgram(ColouredProgram->GetProgramID());

	// Set the PVM matrix
	glUniformMatrix4fv(ColouredProgram->GetUniform("PVM"), 1, GL_FALSE, glm::value_ptr(pvm));

	// Set the colour
	glUniform4f(ColouredProgram->GetUniform("objColour"), colour.x, colour.y, colour.z, 1.0f);

	// Draw the rectangle
	RectangleMesh->Draw();

	// Unbind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Unbind the program
	glUseProgram(0);
}

void GFX::DrawRectangle(glm::mat4& pv, glm::vec4 colour, glm::vec2 aLoc, glm::vec2 aScale)
{
	glm::mat4 pvm = pv * CalculateModelMatrix(aLoc, aScale);

	// Select program
	glUseProgram(ColouredProgram->GetProgramID());

	// Set the PVM matrix
	glUniformMatrix4fv(ColouredProgram->GetUniform("PVM"), 1, GL_FALSE, glm::value_ptr(pvm));

	// Set the colour
	glUniform4f(ColouredProgram->GetUniform("objColour"), colour.x, colour.y, colour.z, colour.w);

	// Draw the rectangle
	RectangleMesh->Draw();

	// Unbind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Unbind the program
	glUseProgram(0);
}

void GFX::DrawRectangle(glm::mat4& pv, glm::vec3 colour, glm::vec2 aLoc, glm::vec2 aScale)
{
	glm::mat4 pvm = pv * CalculateModelMatrix(aLoc, aScale);

	// Select program
	glUseProgram(ColouredProgram->GetProgramID());

	// Set the PVM matrix
	glUniformMatrix4fv(ColouredProgram->GetUniform("PVM"), 1, GL_FALSE, glm::value_ptr(pvm));

	// Set the colour
	glUniform4f(ColouredProgram->GetUniform("objColour"), colour.x, colour.y, colour.z, 1.0f);

	// Draw the rectangle
	RectangleMesh->Draw();

	// Unbind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Unbind the program
	glUseProgram(0);
}

void GFX::DrawLine(glm::mat4& pv, glm::vec3 colour, glm::vec2 p1, glm::vec2 p2)
{
	glm::mat4 pivot = glm::translate(glm::mat4(), glm::vec3(0.0f, -0.5f, 0.0f));
	glm::mat4 scale = glm::scale(glm::mat4(), glm::vec3(glm::length(p1 - p2), 2.0f, 1.0f));
	glm::mat4 rotation = glm::rotate(glm::mat4(), HELPER::PointDirectionRad(p1, p2), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 location = glm::translate(glm::mat4(), glm::vec3(p1.x, p1.y, 0.0f));

	glm::mat4 pvm = pv * location * rotation * scale * pivot;

	// Select program
	glUseProgram(ColouredProgram->GetProgramID());

	// Set the PVM matrix
	glUniformMatrix4fv(ColouredProgram->GetUniform("PVM"), 1, GL_FALSE, glm::value_ptr(pvm));

	// Set the colour
	glUniform4f(ColouredProgram->GetUniform("objColour"), colour.x, colour.y, colour.z, 1.0f);

	// Draw the rectangle
	RectangleMesh->Draw();

	// Unbind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Unbind the program
	glUseProgram(0);
}

void GFX::DrawLine(glm::mat4& pv, glm::vec3 colour, glm::vec2 p1, glm::vec2 p2, float width)
{
	glm::mat4 pivot = glm::translate(glm::mat4(), glm::vec3(0.0f, -0.5f, 0.0f));
	glm::mat4 scale = glm::scale(glm::mat4(), glm::vec3(glm::length(p1 - p2), width, 1.0f));
	glm::mat4 rotation = glm::rotate(glm::mat4(), HELPER::PointDirectionRad(p1, p2), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 location = glm::translate(glm::mat4(), glm::vec3(p1.x, p1.y, 0.0f));

	glm::mat4 pvm = pv * location * rotation * scale * pivot;

	// Select program
	glUseProgram(ColouredProgram->GetProgramID());

	// Set the PVM matrix
	glUniformMatrix4fv(ColouredProgram->GetUniform("PVM"), 1, GL_FALSE, glm::value_ptr(pvm));

	// Set the colour
	glUniform4f(ColouredProgram->GetUniform("objColour"), colour.x, colour.y, colour.z, 1.0f);

	// Draw the rectangle
	RectangleMesh->Draw();

	// Unbind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Unbind the program
	glUseProgram(0);
}

void GFX::DrawLine(glm::mat4& pv, glm::vec4 colour, glm::vec2 p1, glm::vec2 p2)
{
	glm::mat4 pivot = glm::translate(glm::mat4(), glm::vec3(0.0f, -0.5f, 0.0f));
	glm::mat4 scale = glm::scale(glm::mat4(), glm::vec3(glm::length(p1 - p2), 2.0f, 1.0f));
	glm::mat4 rotation = glm::rotate(glm::mat4(), HELPER::PointDirectionRad(p1, p2), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 location = glm::translate(glm::mat4(), glm::vec3(p1.x, p1.y, 0.0f));

	glm::mat4 pvm = pv * location * rotation * scale * pivot;

	// Select program
	glUseProgram(ColouredProgram->GetProgramID());

	// Set the PVM matrix
	glUniformMatrix4fv(ColouredProgram->GetUniform("PVM"), 1, GL_FALSE, glm::value_ptr(pvm));

	// Set the colour
	glUniform4f(ColouredProgram->GetUniform("objColour"), colour.x, colour.y, colour.z, colour.w);

	// Draw the rectangle
	RectangleMesh->Draw();

	// Unbind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Unbind the program
	glUseProgram(0);
}

void GFX::DrawLine(glm::mat4& pv, glm::vec4 colour, glm::vec2 p1, glm::vec2 p2, float width)
{
	glm::mat4 pivot = glm::translate(glm::mat4(), glm::vec3(0.0f, -0.5f, 0.0f));
	glm::mat4 scale = glm::scale(glm::mat4(), glm::vec3(glm::length(p1 - p2), width, 1.0f));
	glm::mat4 rotation = glm::rotate(glm::mat4(), HELPER::PointDirectionRad(p1, p2), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 location = glm::translate(glm::mat4(), glm::vec3(p1.x, p1.y, 0.0f));

	glm::mat4 pvm = pv * location * rotation * scale * pivot;

	// Select program
	glUseProgram(ColouredProgram->GetProgramID());

	// Set the PVM matrix
	glUniformMatrix4fv(ColouredProgram->GetUniform("PVM"), 1, GL_FALSE, glm::value_ptr(pvm));

	// Set the colour
	glUniform4f(ColouredProgram->GetUniform("objColour"), colour.x, colour.y, colour.z, colour.w);

	// Draw the rectangle
	RectangleMesh->Draw();

	// Unbind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Unbind the program
	glUseProgram(0);
}

void GFX::DrawPoints(glm::mat4& pv, glm::vec4 colour, std::vector<glm::vec2> points, float width)
{
	IfThenReturn(points.size() == 0, );
	for (int i = 0; i < (int)points.size(); ++i)
	{
		GFX::DrawRectangle(pv, colour, points[i] - width * 0.5f, { width, width });
	}
}

void GFX::DrawPolygon(glm::mat4& pv, glm::vec4 colour, std::vector<glm::vec2> points, float width)
{
	IfThenReturn(points.size() == 0, );
	glm::vec2 previous = points[0];

	for (int i = 1; i < (int)points.size(); ++i)
	{
		GFX::DrawLine(pv, colour, previous, points[i], width);
		previous = points[i];
	}
	
	GFX::DrawLine(pv, colour, previous, points[0], width);
}

void GFX::DrawString(glm::mat4 pv, CFont* font, std::string text, glm::ivec2 location, glm::vec3 colour, float scale)
{
	glUseProgram(TextProgram->GetProgramID());
	glUniform4f(TextProgram->GetUniform("textColor"), colour.x, colour.y, colour.z, 1.0f);
	glUniformMatrix4fv(TextProgram->GetUniform("proj"), 1, GL_FALSE, glm::value_ptr(pv));
	glBindVertexArray(TextVAO);

	GLuint texID = TextProgram->GetUniform("tex");

	GLfloat xOffset = 0.0f;
	GLfloat yOffset = (GLfloat)font->TextBounds(text, scale).y;

	for (std::string::const_iterator character = text.begin(); character != text.end(); character++)
	{
		FontChar fontChar = font->Characters[*character];

		if (*character == '\n')
		{
			yOffset += font->Size * scale;
			xOffset = 0.0f;
			continue;
		}
		GLfloat xpos = location.x + fontChar.Bearing.x * scale + xOffset;
		GLfloat ypos = location.y - fontChar.Bearing.y * scale + yOffset;
		GLfloat charWidth = fontChar.Size.x * scale;
		GLfloat charHeight = fontChar.Size.y * scale;

		std::vector<GLfloat> vertices = {
			xpos				, ypos				, 0.0, 0.0,
			xpos				, ypos + charHeight	, 0.0, 1.0,
			xpos + charWidth	, ypos + charHeight	, 1.0, 1.0,
			xpos + charWidth	, ypos				, 1.0, 0.0,
		};

		glBindBuffer(GL_ARRAY_BUFFER, TextVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * vertices.size(), vertices.data());

		// Render the glyph texture over the quad
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fontChar.TextureID);
		glUniform1i(texID, 0);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Advance cursors for the next glyph
		xOffset += (fontChar.Advance >> 6)* scale;
	}

	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GFX::DrawString(glm::mat4 pv, CFont* font, std::string text, glm::ivec2 location, glm::vec4 colour, float scale)
{
	glUseProgram(TextProgram->GetProgramID());
	glUniform4f(TextProgram->GetUniform("textColor"), colour.x, colour.y, colour.z, colour.w);
	glUniformMatrix4fv(TextProgram->GetUniform("proj"), 1, GL_FALSE, glm::value_ptr(pv));
	glBindVertexArray(TextVAO);

	GLuint texID = TextProgram->GetUniform("tex");

	GLfloat xOffset = 0.0f;
	GLfloat yOffset = (GLfloat)font->TextBounds(text, scale).y;

	for (std::string::const_iterator character = text.begin(); character != text.end(); character++)
	{
		FontChar fontChar = font->Characters[*character];

		if (*character == '\n')
		{
			yOffset += font->Size * scale;
			xOffset = 0.0f;
			continue;
		}
		GLfloat xpos = location.x + fontChar.Bearing.x * scale + xOffset;
		GLfloat ypos = location.y - fontChar.Bearing.y * scale + yOffset;
		GLfloat charWidth = fontChar.Size.x * scale;
		GLfloat charHeight = fontChar.Size.y * scale;

		std::vector<GLfloat> vertices = {
			xpos				, ypos				, 0.0, 0.0,
			xpos				, ypos + charHeight	, 0.0, 1.0,
			xpos + charWidth	, ypos + charHeight	, 1.0, 1.0,
			xpos + charWidth	, ypos				, 1.0, 0.0,
		};

		glBindBuffer(GL_ARRAY_BUFFER, TextVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * vertices.size(), vertices.data());

		// Render the glyph texture over the quad
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fontChar.TextureID);
		glUniform1i(texID, 0);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Advance cursors for the next glyph
		xOffset += (fontChar.Advance >> 6) * scale;
	}

	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}


glm::mat4 GFX::CalculateModelMatrix(glm::vec2 translation, glm::vec2 scale)
{
	glm::mat4 scaleMat = glm::scale(glm::mat4(), glm::vec3(scale, 1.0f));
	glm::mat4 translationMat = glm::translate(glm::mat4(), glm::vec3(translation, 0.0f));

	return translationMat * scaleMat;
}

glm::mat4 GFX::CalculateModelMatrix(glm::vec2 translation, glm::vec2 scale, glm::vec2 pivot)
{
	glm::mat4 pivotMat = glm::translate(glm::mat4(), glm::vec3(pivot, 0.0f));
	glm::mat4 scaleMat = glm::scale(glm::mat4(), glm::vec3(scale, 1.0f));
	glm::mat4 translationMat = glm::translate(glm::mat4(), glm::vec3(translation, 0.0f));

	return translationMat * scaleMat * pivotMat;
}

glm::mat4 GFX::CalculateModelMatrix(glm::vec2 translation, glm::vec2 scale, float rotationZ)
{
	glm::mat4 scaleMat = glm::scale(glm::mat4(), glm::vec3(scale, 1.0f));
	glm::mat4 rotationMat = glm::rotate(glm::mat4(), glm::radians(rotationZ), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 translationMat = glm::translate(glm::mat4(), glm::vec3(translation, 0.0f));

	return translationMat * rotationMat * scaleMat;
}

glm::mat4 GFX::CalculateModelMatrix(glm::vec3 translation, glm::vec3 scale)
{
	glm::mat4 scaleMat = glm::scale(glm::mat4(), scale);
	glm::mat4 translationMat = glm::translate(glm::mat4(), translation);

	return translationMat * scaleMat;
}

glm::mat4 GFX::CalculateModelMatrix(glm::vec3 translation, glm::vec3 scale, float rotationZ)
{
	glm::mat4 scaleMat = glm::scale(glm::mat4(), scale);
	glm::mat4 rotationMat = glm::rotate(glm::mat4(), glm::radians(rotationZ), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 translationMat = glm::translate(glm::mat4(), translation);

	return translationMat * rotationMat * scaleMat;
}

glm::mat4 GFX::CalculateModelMatrix(glm::vec3 translation, glm::vec3 scale, float rotationX, float rotationY, float rotationZ)
{
	glm::mat4 scaleMat = glm::scale(glm::mat4(), scale);
	glm::mat4 rotationXMat = glm::rotate(glm::mat4(), glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotationYMat = glm::rotate(glm::mat4(), glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotationZMat = glm::rotate(glm::mat4(), glm::radians(rotationZ), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 translationMat = glm::translate(glm::mat4(), translation);

	return translationMat * rotationZMat * rotationYMat * rotationXMat * scaleMat;
}
