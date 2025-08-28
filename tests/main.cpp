#include <ExtBasicIncludes.h>
#include <Core/MainWindow.h>
#include <Render/DrawContext.h>
#include <Image/Color.h>
#include <Image/Image.h>
#include <Image/ImageLoader.h>
#include <Render/Texture2D.h>

int main(int argc, char *argv[])
{
	core::MainWindowParameters params;
	params.Caption = L"Test IrrlichtRedo App";

	core::InfoStream << "Init Window\n";
	std::unique_ptr<core::MainWindow> window = std::make_unique<core::MainWindow>(params);
	core::InfoStream << "Inited Window\n";

	v2u wndsize = window->getWindowSize();
	auto glparams = window->getGLParams();
	core::InfoStream << "Init DrawContext\n";
	auto context = std::make_unique<render::DrawContext>(recti(0,0,wndsize.X,wndsize.Y), glparams.maxTextureUnits);
	core::InfoStream << "Create clear color\n";
	img::color8 clear(img::PF_RGBA8, 128, 60, 175, 255);

	//core::InfoStream << "Output clear color: r=" << clear.R() << ", g=" << clear.G() << ", b=" << clear.B() << ", a=" << clear.A() << "\n";

	//clear.G(35);
	//clear.A(0);
	//core::InfoStream << "Output clear color: r=" << clear.R() << ", g=" << clear.G() << ", b=" << clear.B() << ", a=" << clear.A() << "\n";

	img::ImageLoader::init();

	auto img = img::ImageLoader::load("icon.png");

	render::TextureSettings settings;
	settings.isRenderTarget = false;
	auto texture = std::make_unique<render::Texture2D>("MT Image", std::unique_ptr<img::Image>(img), settings);

	core::InfoStream << "texture width: " << texture->getWidth() << ", height: " << texture->getHeight() << "\n";

	core::InfoStream << "Run loop\n";
	while (window->pollEventsFromQueue()) {
		core::InfoStream << "Loop...\n";
		context->clearBuffers(render::CBF_COLOR | render::CBF_DEPTH, img::color8(img::PF_RGBA8, 255, 0, 0, 255));
		core::InfoStream << "Cleared the framebuffer\n";



		window->SwapWindow();
	}

	img::ImageLoader::free();

	return 0;
}
