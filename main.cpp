#include <iostream>
#include <vector>

#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Image : public GLFWimage {
	explicit Image(const char* filename) : GLFWimage{} {
		int channels;
		auto* data = stbi_load(filename, &width, &height, &channels, 4);
		size_t size = width * height * 4;
		pixels = new unsigned char[size];
		memcpy(pixels, data, size);
		stbi_image_free(data);
	}

	Image(const Image& front, const Image& back) : GLFWimage{} {
		width = std::max(front.width, back.width);
		height = std::max(front.height, back.height);

		size_t size = width * height * 4;
		pixels = new unsigned char[size];
		memset(pixels, 0, size);

		int xoffset = width / 2 - back.width / 2;
		int yoffset = height / 2 - back.height / 2;

		for (int row = 0; row < back.height; row++) {
			int di = ((row + yoffset) * width + xoffset) * 4;
			int si = row * back.width * 4;
			memcpy(pixels + di, back.pixels + si, back.width * 4);
		}

		xoffset = width / 2;
		yoffset = height / 2;

		for (int row = 0; row < front.height; row++) {
			int di = ((row + yoffset) * width + xoffset) * 4;
			int si = row * front.width * 4;
			memcpy(pixels + di, front.pixels + si, front.width * 4);
		}
	}

	~Image() { delete[] pixels; }
};

struct UserData {
	GLFWcursor* current;
	Image cursor;
	Image texture;
	bool drawItem{true};
	struct { double x{}; double y{}; } pos;
};

void errorCallback(int error, const char* description);
void onMouseButton(GLFWwindow* window, int button, int action, int);

float viewSize = 512;
float quadSize = 128;

int main() {
	glfwSetErrorCallback(errorCallback);

	if (!glfwInit()) { return 1; }

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	auto* window = glfwCreateWindow((int) viewSize, (int) viewSize, "Hardware Cursor", nullptr, nullptr);
	if (!window) { return 1; }

	glfwMakeContextCurrent(window);
	glViewport(0, 0, viewSize, viewSize);
	glfwSwapInterval(0);

	UserData data{.cursor = Image("../cursor.png"), .texture = Image("../texture.png")};

	glfwSetWindowUserPointer(window, &data);
	glfwSetMouseButtonCallback(window, onMouseButton);

	GLFWcursor* cursor = glfwCreateCursor(&data.cursor, 0, 0);
	glfwSetCursor(window, cursor);

	glClearColor(0, 0, 0, 1);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		if (data.drawItem) {
			glBegin(GL_QUADS);
			glColor4f(1, 0, 0, 1);
			glVertex2d(data.pos.x, data.pos.y);
			glVertex2d(data.pos.x, data.pos.y + 0.5);
			glVertex2d(data.pos.x + 0.5, data.pos.y + 0.5);
			glVertex2d(data.pos.x + 0.5, data.pos.y);
			glEnd();
		}

		glfwSwapBuffers(window);
	}

	glfwTerminate();
}

void errorCallback(int error, const char* description) {
	std::cerr << "Error (" << error << "): " << description << std::endl;
}

void onMouseButton(GLFWwindow* window, int button, int action, int) {
	auto data = reinterpret_cast<UserData*>(glfwGetWindowUserPointer(window));
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			auto* old = data->current;
			auto i = Image{data->cursor, data->texture};
			data->current = glfwCreateCursor(&i, i.width / 2, i.height / 2);
			glfwSetCursor(window, data->current);
			data->drawItem = false;
			if (old) { glfwDestroyCursor(old); }
		} else if (action == GLFW_RELEASE) {
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			data->pos.x = (x - quadSize / 2) / viewSize * 2 - 1;
			data->pos.y = -((y + quadSize / 2) / viewSize * 2 - 1);

			auto* old = data->current;
			data->current = glfwCreateCursor(&data->cursor, 0, 0);
			glfwSetCursor(window, data->current);
			data->drawItem = true;
			if (old) { glfwDestroyCursor(old); }
		}
	}
}
