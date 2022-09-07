#include "font_opener.h"
#include "message_box.h"
#include "sdl_initializer.h"
#include "sdl_window_creator.h"
#include "ttf_initializer.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <cstddef>
#include <stdbool.h>
#include <stdint.h>
#include <vector>

static const size_t SCREEN_COLS = 800;
static const size_t SCREEN_ROWS = 600;

struct rgba {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
};

struct screen_state {
	// We are scaling not saving the cursor, but saving the
	// center of the screen. It's means less precision losses.
	struct {
		float x;
		float y;
	} center;
	
	// Difference in coordinates if moving to an adjancent cell.
	float scale;
};


// Have (0, 0) at the center.
static const struct screen_state INITIAL_SCREEN_STATE = {{0, 0}, 1};

// We are using powers of 2 here as it's bringing no precision losses
// while stepping and dividing and multiplying by 2 also brings no
// losses in precision, which helps us to save the center of the
// picture (it's coordinates x and y, not gather precision losses).
// Center of the picture is at col = SCREEN_COLS / 2 and
// row = SCREEN_ROWS / 2.
static const int SCALE_TIMES_STEP = 2;
static const int PIXEL_STEP = 32;

// Часть окна, которая способна принимать фокус и становиться активной по сравнению с остальными.
class FocusableRegion {
public:
	FocusableRegion(int top_left_row, int top_left_col, int width, int height)
	  : top_left_row_(top_left_row),
	    top_left_col_(top_left_col),
	    width_(width),
	    height_(height),
	    is_active_(false) {
	  	
	}
	
	virtual ~FocusableRegion() {}
	
	void activate() {
		is_active_ = true;
	}
	
	virtual draw(SDL_Renderer* renderer) = 0;
	
	virtual void process_event(SDL_Event& event) {
		(void) event;
		// if (event.type == 
	}
private:
	int top_left_row_;
	int top_left_col_;
	int width_;
	int height_;
	
	bool is_active_;
};

class CoordinateSystem : public FocusableRegion {
public:
	CoordinateSystem(int top_left_row, int top_left_col, int width, int height)
	  : FocusableRegion(top_left_row, top_left_col, width, height) {}
	  
	virtual void draw(SDL_Renderer* renderer) {
	}

	virtual ~CoordinateSystem() {
		
	}
private:
	
};

template<typename T>
class shared_ptr;

class shared_ptr_ref_counter {
private:
	shared_ptr_ref_counter()
	  : num_refs_(0) {}
	
	void inc() {
		num_refs_ += 1;
	}
	
	void dec() {
		num_refs_ -= 1;
	}
	
	size_t value() const {
		return num_refs_;
	}
	
	template<typename T>
	friend class shared_ptr;
private:
	size_t num_refs_;
};

template<typename T>
class shared_ptr {
public:
	shared_ptr()
	  : ptr_(NULL), ref_counter_(NULL) {}

	shared_ptr(T* ptr)
	  : ptr_(ptr), ref_counter_(NULL) {
		if (ptr_ != NULL) {
			ref_counter_ = new shared_ptr_ref_counter();
			ref_counter_->inc();
		}
	}

	// For construction out of variables.
	shared_ptr(shared_ptr& other)
	  : ptr_(NULL), ref_counter_(NULL) {
		copy(other);
	}

	// For construction out of temporary objects. From C++11 onwards also binds remove references.
	// Should also work for variable initialization (also bind to non-const references), it
	// just doesn't work in C++03, but should work from C++11 onwards.
	shared_ptr(const shared_ptr& other)
	  : ptr_(NULL), ref_counter_(NULL) {
		copy(other);
	}
	
	// For assignment with rhs being a temporary object.
	// Should also work for a variable (bind to a simple
	// reference, although object is convertible to
	// const reference as well, it just doens't work in c++03),
	// from c++11 onwards.
	shared_ptr& operator=(const shared_ptr& other) {
		copy(other);
	}

	// For assignment with rhs being a variable.
	shared_ptr& operator=(shared_ptr& other) {
		copy(other);
	}

public:
	
	T* operator->() {
		return ptr_;
	}
	
	T* get() {
		return ptr_;
	}
	
	operator bool() {
		return ptr_ != NULL;
	}
	
	~shared_ptr() {
		unref();
	}
private:
	void copy(const shared_ptr& other) {
		unref();
		
		ptr_ = other.ptr_;
		ref_counter_ = other.ref_counter_;

		assert(other.ptr_ != NULL || other.ref_counter_ == NULL);
		if (ptr_ != NULL) {
			ref_counter_->inc();
		}
	}
	void unref() {
		if (ptr_ != NULL) {
			ref_counter_->dec();
		 	if (ref_counter_->value() == 0) {
				delete ptr_;
				delete ref_counter_;
				ptr_ = NULL;
				ref_counter_ = NULL;
			}
		} else {
			assert(ptr_ == NULL && ref_counter_ == NULL);
		}

		ptr_ = NULL;
		ref_counter_ = NULL;
	}
private:
	T* ptr_;
	shared_ptr_ref_counter* ref_counter_;
};

class PlotWindow {
public:
	static shared_ptr<PlotWindow> create() {
		shared_ptr<PlotWindow> plot_window(new PlotWindow());
		if (plot_window->window_owner.get() == NULL) {
			show_message_box(MESSAGEBOX_ERROR, "Ошибка SDL2", "Ошибка при создании окна. Подробнее в логе (stderr).", NULL);
			return shared_ptr<PlotWindow>();
		}
		assert(WIDTH % NUM_REGIONS == 0);

		// We cannot construct temporary unique_ptr and then push_back them into the vector.
		// While we delete the temporary object, we free the allocated data.
		// We cannot workaround this with copying constructor, as non-const reference doesn't
		// bind to temporary objects.
		// Moving constructor is not present until c++11, I suppose.
		// We cannot implement unique_ptr in c++03, but we can implement shared_ptr.

		for (size_t i = 0; i < NUM_REGIONS; ++i) {
			plot_window->regions.push_back(shared_ptr<FocusableRegion>(new CoordinateSystem(0, WIDTH / NUM_REGIONS * i, WIDTH / NUM_REGIONS, HEIGHT))); // top_row, top_col, width, height
		}
		assert(NUM_REGIONS != 0);
		plot_window->regions[0]->activate();
		
		return plot_window;
	}
	
	void pass_event_to_regions(SDL_Event& event) {
		for (size_t i = 0; i < regions.size(); ++i) {
			regions[i]->process_event(event);
		}
	}
	
	void process_event(SDL_Event& event) {
		switch (event.type) {
			case SDL_QUIT: {
				exiting = true;
				break;
			}
					
			default: {
				pass_event_to_regions(event);
			}
		}
	}
	
	void process_events() {
		// TODO: not fps counter, but frame time box.

		while (!exiting) {
			SDL_Event event = {0};
			while (SDL_PollEvent(&event)) {
				process_event(event);
			}
		}
				
		//int row_size = 0;                                              // In bytes
		//struct rgba* pixels = NULL;
		/*
		SDL_LockTexture(texture, NULL, (void**) &pixels, &row_size);
		memset(pixels, 0, sizeof(struct rgba) * SCREEN_ROWS * SCREEN_COLS);
		SDL_UnlockTexture(texture);
		*/
		
		//SDL_RenderClear(renderer);
		//SDL_RenderCopy(renderer, texture, NULL, NULL);
		//SDL_RenderPresent(renderer);

		// uint64_t time_ended = SDL_GetPerformanceCounter();
		
		// prev_frame_time = (time_ended - time_started) / ((float) SDL_GetPerformanceFrequency());
	}
	
	//SDL_DestroyTexture(texture);

	//}
private:
	PlotWindow()
	  : window_owner(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN),
	    exiting(false) {}
private:
	SdlWindowOwner window_owner;
	std::vector<shared_ptr<FocusableRegion> > regions;
	
	bool exiting;
	
	static const char* const TITLE;
	static const int WIDTH = 600;
	static const int HEIGHT = 350;
	static const size_t NUM_REGIONS = 2;
};
const char* const PlotWindow::TITLE = "Системы координат";

class PlotInputWindow {
public:
private:
};

// Workaround strange SDL2 behaviour when it redefines main function, but then with -lSDL2Main -lSDL2 it doesn't compile.
// Based on https://stackoverflow.com/a/29848468
#if defined(main)
#undef main
#endif

int main() {
	SdlInitializer sdl_initializer(SDL_INIT_VIDEO);
	if (!sdl_initializer.initialized()) {
		show_message_box(MESSAGEBOX_ERROR, "Ошибка SDL2", "Ошибка при инициализации. Подробнее в логе (stderr).", NULL);
		return 1;
	}
	
	TtfInitializer ttf_initializer;
	if (!ttf_initializer.initialized()) {
		show_message_box(MESSAGEBOX_ERROR, "Ошибка SDL2_TTF", "Ошибка при инициализации библиотеки для работы со шрифтами. Подробнее в логе (stderr).", NULL);
		return 2;
	}
/*	
	SDL_Window* window = window_owner.get();
	if (window == NULL) {
		return 3;
	}
*/	
	// show_message_box(MESSAGEBOX_ERROR, "1", "2", window);
/*
	SdlRendererCreator renderer_creator(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_Renderer* renderer = renderer_creator.get();
	if (renderer == NULL) {
		show_message_box(MESSAGEBOX_ERROR, "Ошибка SDL2", "Ошибка при создании отрисовщика. Подробнее в логе (stderr).", window);*/
		//return 3; // RETURN_CODE_FAILED_TO_CREATE_RENDERER
	//}
	
	// Looks like SDL orders these fields big-endian, so for the structure those will be RGBA.
	/*SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_COLS, SCREEN_ROWS);
	if (texture == NULL) {
		fprintf(stderr, "Failed to create texture. SDL_CreateTexture failed: %s.\n", SDL_GetError());
		show_message_box(MESSAGEBOX_ERROR, "Ошибка SDL2", "Ошибка при создании текстуры. Подробнее в логе (stderr).", window);
		return 5;
	}*/
	
	static const char AXIS_FONT_FILE[] = "ubuntu-mono/UbuntuMono-Regular.ttf";
	static const int AXIS_FONT_SIZE = 32; // In points.
	FontOpener axis_font_opener(AXIS_FONT_FILE, AXIS_FONT_SIZE);
	TTF_Font* axis_font = axis_font_opener.get();
	if (axis_font == NULL) {
		//show_message_box(MESSAGEBOX_ERROR, "Ошибка SDL2_TTF", "Ошибка при загрузке шрифта для осей. Подробнее в логе (stderr).", window);
		// SDL_DestroyTexture(texture);
		return 6;
	}
	
	shared_ptr<PlotWindow> plot_window = PlotWindow::create();
	if (!plot_window) {
		//show_message_box(MESSAGEBOX_ERROR, "Ошибка", "Ошибка при создании окна. Подробнее в логе (stderr).", window);
		return 7;
	}
	
//	struct screen_state screen_state = INITIAL_SCREEN_STATE;	
/*
	bool exiting = false;
	while (!exiting) {
		// uint64_t time_started = SDL_GetPerformanceCounter();
	
		SDL_Event event = {0};
*/
/*		
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_KEYDOWN: {
					switch (event.key.keysym.scancode) {
						// TODO: consider precision losses here by limiting scaling and moving.
						case SDL_SCANCODE_RETURN: {
							screen_state.scale /= SCALE_TIMES_STEP;
							break;
						}
						
						case SDL_SCANCODE_SPACE: {
							screen_state.scale *= SCALE_TIMES_STEP;
							break;
						}
						
						case SDL_SCANCODE_LEFT: {
							screen_state.center.x -= PIXEL_STEP * screen_state.scale;
							break;
						}
						
						case SDL_SCANCODE_RIGHT: {
							screen_state.center.x += PIXEL_STEP * screen_state.scale;
							break;
						}
						
						case SDL_SCANCODE_UP: {
							screen_state.center.y += PIXEL_STEP * screen_state.scale;
							break;
						}

						case SDL_SCANCODE_DOWN: {
							screen_state.center.y -= PIXEL_STEP * screen_state.scale;
							break;
						}
						
						case SDL_SCANCODE_TAB: {
							break;
						}
						
						case SDL_SCANCODE_LALT:
						case SDL_SCANCODE_RALT: {
							break;
						}
						
						default: break;
					}
					break;
				}
				
				case SDL_QUIT: {
					exiting = true;
					break;
				}
			}
		}*/
/*				
		int row_size = 0;                                              // In bytes
		struct rgba* pixels = NULL;
		SDL_LockTexture(texture, NULL, (void**) &pixels, &row_size);
		memset(pixels, 0, sizeof(struct rgba) * SCREEN_ROWS * SCREEN_COLS);
		SDL_UnlockTexture(texture);
		
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
*/
		// uint64_t time_ended = SDL_GetPerformanceCounter();
		
		// prev_frame_time = (time_ended - time_started) / ((float) SDL_GetPerformanceFrequency());
//	}
	
//	SDL_DestroyTexture(texture);

	return 0;
}
/*
				while (SDL_PollEvent(&event)) {
					switch (event.type) {
					case SDL_KEYDOWN: {
						switch (event.key.keysym.scancode) {*/
							// TODO: consider precision losses here by limiting scaling and moving.
/*							case SDL_SCANCODE_RETURN: {
								screen_state.scale /= SCALE_TIMES_STEP;
								break;
							}
						
							case SDL_SCANCODE_SPACE: {
								screen_state.scale *= SCALE_TIMES_STEP;
								break;
							}
						
							case SDL_SCANCODE_LEFT: {
								screen_state.center.x -= PIXEL_STEP * screen_state.scale;
								break;
							}
						
							case SDL_SCANCODE_RIGHT: {
								screen_state.center.x += PIXEL_STEP * screen_state.scale;
								break;
							}
						
							case SDL_SCANCODE_UP: {
								screen_state.center.y += PIXEL_STEP * screen_state.scale;
								break;
						}

						case SDL_SCANCODE_DOWN: {
							screen_state.center.y -= PIXEL_STEP * screen_state.scale;
							break;
						}
						
						case SDL_SCANCODE_TAB: {
							break;
						}
						
						case SDL_SCANCODE_LALT:
						case SDL_SCANCODE_RALT: {
							break;
						}
						
						default: break;
					}
					break;
				}*/
				// uint64_t time_started = SDL_GetPerformanceCounter();