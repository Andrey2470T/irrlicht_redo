#include <Core/MainWindow.h>
#include <Render/DrawContext.h>
#include <Image/Color.h>
#include <Image/Image.h>
#include <Image/ImageLoader.h>
#include <Render/Texture2D.h>
#include <Render/Shader.h>
#include <Render/Mesh.h>

void setVertex(ByteArray &ba, v2f pos, img::color8 c, v2f uv, u32 n)
{
    ba.setFloat(pos.X, n);
    ba.setFloat(pos.Y, n+1);

    ba.setUInt8(c.R(), n+2);
    ba.setUInt8(c.G(), n+3);
    ba.setUInt8(c.B(), n+4);
    ba.setUInt8(c.A(), n+5);

    ba.setFloat(uv.X, n+6);
    ba.setFloat(uv.Y, n+7);
}
int main(int argc, char *argv[])
{
    render::VertexTypeDescriptor VType2D{
        "Standard2D",
        {},
        2,
        4,
        false,
        true,
        2
    };

	core::MainWindowParameters params;
	params.Caption = L"Test IrrlichtRedo App";

    core::InfoStream << "Init Window\n";
    core::MainWindow window(params);
    core::InfoStream << "Inited Window\n";


    v2u wndsize = window.getWindowSize();
    auto glparams = window.getGLParams();
    core::InfoStream << "Init DrawContext: maxTextureUnits=" << glparams->maxTextureUnits << "\n";
    auto context = std::make_unique<render::DrawContext>(recti(0,0,wndsize.X,wndsize.Y), glparams->maxTextureUnits);
	core::InfoStream << "Create clear color\n";
	img::color8 clear(img::PF_RGBA8, 128, 60, 175, 255);

    core::InfoStream << "Output clear color: r=" << clear.R() << ", g=" << clear.G() << ", b=" << clear.B() << ", a=" << clear.A() << "\n";

    clear.G(35);
    clear.A(0);
    core::InfoStream << "Output clear color: r=" << clear.R() << ", g=" << clear.G() << ", b=" << clear.B() << ", a=" << clear.A() << "\n";

	core::InfoStream << "Init ImageLoader...\n";
    img::ImageLoader::init();

	core::InfoStream << "Load the icon.png...\n";

    fs::path abs_icon_p = fs::absolute("icon.png");
	core::InfoStream << "icon.png path:" << abs_icon_p << "\n";
    auto img = img::ImageLoader::load(abs_icon_p.string());

    img::ImageModifier imgmod;
    imgmod.setBlendMode(img::BM_MULTIPLY);
    imgmod.fill(img, img::red);
    //imgmod.setBlendMode(img::BM_NORMAL);
    //auto img2 = imgmod.rotate(img, img::RA_270);
    //delete img;
    v2u curSize = img->getSize();
    v2u newSize = img->getSize() / 2;

    /*fs::path abs_icon_p2 = fs::absolute("default_cobble.png");
    auto img2 = img::ImageLoader::load(abs_icon_p2.string());
    imgmod.setBlendMode(img::BM_ADD);
    auto img3 = imgmod.combine(img, img2);
    delete img;
    delete img2;*/
    imgmod.resize(&img, rectu(v2u(0), newSize.X, newSize.Y));
    //imgmod.setBlendMode(img::BM_ADD);
    //imgmod.fill(img, img::red);
    //imgmod.setBlendMode(img::BM_NORMAL);
    //auto fimg = imgmod.rotate(img, img::RA_180);
    //auto fimg = imgmod.flip(img, img::FD_X);
    //delete img;


    /*for (u32 x = 0; x < img->getWidth(); x++)
        for (u32 y = 0; y < img->getHeight(); y++) {
            auto curColor = imgmod.getPixelColor(img, x, y);
            imgmod.setPixel(img, x, y, img::color8(img::PF_RGBA8, curColor.R()+255, curColor.G(), curColor.B(), curColor.A()));
        }*/

	core::InfoStream << "Setup and create the texture...\n";
	render::TextureSettings settings;
	settings.isRenderTarget = false;
    auto texture = std::make_unique<render::Texture2D>("MT Image", std::unique_ptr<img::Image>(img), settings);

    auto shader = std::make_unique<render::Shader>(fs::absolute("shader2d_vertex.glsl"), fs::absolute("shader2d_fragment.glsl"));

    InfoStream << "main 0\n";
    ByteArray mesh_vertexdata(8 * 4, render::sizeOfVertexType(VType2D) * 4);

    setVertex(mesh_vertexdata, v2f(-1.0f, 1.0f), img::white, v2f(0.0f, 1.0f), 0);
    setVertex(mesh_vertexdata, v2f(1.0f, 1.0f), img::white, v2f(1.0f, 1.0f), 8);
    setVertex(mesh_vertexdata, v2f(1.0f, -1.0f), img::white, v2f(1.0f, 0.0f), 16);
    setVertex(mesh_vertexdata, v2f(-1.0f, -1.0f), img::white, v2f(0.0f, 0.0f), 24);

    ByteArray mesh_indexdata(6, sizeof(u32) * 6);
    mesh_indexdata.setUInt32(0, 0);
    mesh_indexdata.setUInt32(3, 1);
    mesh_indexdata.setUInt32(2, 2);
    mesh_indexdata.setUInt32(0, 3);
    mesh_indexdata.setUInt32(2, 4);
    mesh_indexdata.setUInt32(1, 5);

    InfoStream << "main 1\n";
    auto color = img::getColor8(&mesh_vertexdata, 2);
    InfoStream << "main 2\n";
    InfoStream << "mesh_vertexdata uv0: " << mesh_vertexdata.getV2F(6) << "\n";
    InfoStream << "mesh_vertexdata uv0: " << mesh_vertexdata.getV2F(6) << "\n";
    InfoStream << "mesh_vertexdata color0: " << color.R() << ", " << color.G() << ", " << color.B() << ", " << color.A() << "\n";
    InfoStream << "mesh_vertexdata pos1: " << mesh_vertexdata.getV2F(8) << "\n";

    auto mesh = std::make_unique<render::Mesh>(
        mesh_vertexdata.data(), mesh_vertexdata.bytesCount(), (u32 *)mesh_indexdata.data(), mesh_indexdata.bytesCount(), VType2D);

    core::InfoStream << "mesh_vertexdata size: " << mesh_vertexdata.bytesCount() << ", mesh_indexdata size: " << mesh_indexdata.bytesCount() << "\n";

    core::InfoStream << "Run loop\n";

    while (window.pollEventsFromQueue()) {
        //core::InfoStream << "Loop...\n";
        context->clearBuffers(render::CBF_COLOR | render::CBF_DEPTH, img::black);
        //core::InfoStream << "Set shader\n";

        context->setShader(shader.get());

       // core::InfoStream << "Set texture\n";
        context->setActiveUnit(0, texture.get());

        //core::InfoStream << "Set mesh\n";
        context->setMesh(mesh.get());

        //core::InfoStream << "Draw\n";
        mesh->draw(render::PT_TRIANGLES, 6);

        window.clearEventQueue();
        window.SwapWindow();
    }

    img::ImageLoader::free();

	return 0;
}
