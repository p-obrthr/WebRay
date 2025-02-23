# old without wasm & websocket support not supported while using emscripten
compile:
	gcc -o main main.cpp libraylib.a -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL -lm -std=c++11

#web
compileWasm:
	emcc -o ./web/index.html main.cpp -Wall -std=c++14 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -Os \
		-I. -I ./raylib/src -I ./raylib/src/external -L. -L ./raylib/src -s USE_GLFW=3 -s ASYNCIFY -s TOTAL_MEMORY=67108864 \
		-s FORCE_FILESYSTEM=1 --preload-file ./graphics --shell-file ./raylib/src/shell.html \
		./libraylib.web.a -DPLATFORM_WEB -s 'EXPORTED_FUNCTIONS=["_free","_malloc","_main"]' \
		-s 'EXPORTED_RUNTIME_METHODS=["ccall","emscripten_websocket_send_utf8_text","emscripten_websocket_is_supported","emscripten_websocket_new","emscripten_websocket_set_onopen_callback","emscripten_websocket_set_onerror_callback","emscripten_websocket_set_onclose_callback","emscripten_websocket_set_onmessage_callback"]' \
		-lhtml5 -s WEBSOCKET_URL="ws://localhost:8080" -lwebsocket.js

serve: 
	python3 -m http.server --directory ./web 8000

all: compileWasm serve
