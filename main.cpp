#include <hge.h>
#include "src/module.h"

HGE *hge=0;

// Quad is the basic primitive in HGE
// used for rendering graphics.
// Quad contains 4 vertices, numbered
// 0 to 3 clockwise.
hgeQuad quad;

// Handle for a sound effect
HEFFECT snd;

Gui::Manager* g_manager;

// Some "gameplay" variables and constants
float x=100.0f, y=100.0f;
float dx=0.0f, dy=0.0f;

const float speed=90;
const float friction=0.98f;

// This function plays collision sound with
// parameters based on sprite position and speed
void boom() {
    int pan=int((x-400)/4);
    float pitch=(dx*dx+dy*dy)*0.0005f+0.2f;
    hge->Effect_PlayEx(snd,100,pan,pitch);
}

Dialog* d;

Gui::PopMenu* menu;

bool FrameFunc()
{
    hgeInputEvent event;
    while (hge->Input_GetEvent(&event))
    {
        switch(event.type)
        {
        case INPUT_MOUSEMOVE:            
            g_manager->OnMouseMove(event);            
            break;
        case INPUT_MOUSEWHEEL:
            g_manager->OnMouseWheel(event);
            break;
        case INPUT_KEYDOWN:
            g_manager->OnKeyDown(event);
            break;
        case INPUT_KEYUP:
            g_manager->OnKeyUp(event);
            break;
        case INPUT_MBUTTONDOWN:
            g_manager->OnMousePress(event);
            if (event.key == HGEK_RBUTTON)
            {
                if (menu)
                {
                    menu->Resize(event.x, event.y, 100, 200);
                    menu->Exec();
                }
            }
            break;
        case INPUT_MBUTTONUP:
            g_manager->OnMouseRelease(event);
        }
    }
    // Get the time elapsed since last call of FrameFunc().
    // This will help us to synchronize on different
    // machines and video modes.
    float dt=hge->Timer_GetDelta();

    g_manager->Update(dt);
    // Process keys
    if (hge->Input_GetKeyState(HGEK_ESCAPE)) return true;
    if (hge->Input_GetKeyState(HGEK_LEFT)) dx-=speed*dt;
    if (hge->Input_GetKeyState(HGEK_RIGHT)) dx+=speed*dt;
    if (hge->Input_GetKeyState(HGEK_UP)) dy-=speed*dt;
    if (hge->Input_GetKeyState(HGEK_DOWN)) dy+=speed*dt;

    // Do some movement calculations and collision detection
    dx*=friction; dy*=friction; x+=dx; y+=dy;
    if(x>784) {x=784-(x-784);dx=-dx;boom();}
    if(x<16) {x=16+16-x;dx=-dx;boom();}
    if(y>584) {y=584-(y-584);dy=-dy;boom();}
    if(y<16) {y=16+16-y;dy=-dy;boom();}

    // Set up quad's screen coordinates
    quad.v[0].x=x-16; quad.v[0].y=y-16;
    quad.v[1].x=x+16; quad.v[1].y=y-16;
    quad.v[2].x=x+16; quad.v[2].y=y+16;
    quad.v[3].x=x-16; quad.v[3].y=y+16;

    // Continue execution
    return false;
}

// This function will be called by HGE when
// the application window should be redrawn.
// Put your rendering code here.
bool RenderFunc()
{
    // Begin rendering quads.
    // This function must be called
    // before any actual rendering.
    hge->Gfx_BeginScene();

    // Clear screen with black color
    hge->Gfx_Clear(0);

    g_manager->Repaint();

    // Render quads here. This time just
    // one of them will serve our needs.
 //   hge->Gfx_RenderQuad(&quad);

    // End rendering and update the screen
    hge->Gfx_EndScene();

    // RenderFunc should always return false
    return false;
}

void CreateWidget()
{
    struct CreateWidgetAction : Gui::Action
    {
        CreateWidgetAction(void (*f)())
            : function(f)
        {}

        void operator() (const Gui::ActionParameter&)
        {
            function();
        }

        void (*function)();
    };


    {
        Gui::VerticalLayout* layout = new  Gui::VerticalLayout();
        Gui::Widget* w = new Gui::Widget(100, 300, 200, 200);
        Gui::Button* w2 = new Gui::Button(10, 10, 180, 20, "Button", w);
        Gui::Button* w4 = new Gui::Button(10, 10, 180, 20, "Button2", w);
        Gui::TextLine* w3 = new Gui::TextLine(10, 40, 180, 20, "Text", w);
        layout->AddWidget(w2);
        layout->AddWidget(w4);
        layout->AddWidget(w3);
        w->SetLayout(layout);
        w2->SigToggle.Connect(new CreateWidgetAction(CreateWidget));
        g_manager->Add(w);
    }
}

void InitGui()
{
    g_manager = new Gui::Manager(hge);
    Gui::ScrollBar* w = new Gui::ScrollBar(400, 300, 10, 250, nullptr);
    Gui::ListBox* l = new Gui::ListBox(450, 100, 200, 400, nullptr);
    g_manager->Add(w);
    std::vector<std::string> names {"Gun&Roses", "Onix", "WuTang Clan", "Zeman", "2H Company", "Nirvana",
                                    "Pixies", "REM", "Lordz of Brooklyn", "Jay-Z", "Lil John", "Black Sabbath",
                                    "Motorhead HAHAHAHAHAHA", "Metallica", "Rammstein", "NRM", "Iron Maden", "Judas Priests",
                                    "Bob Marley", "Run D.M.C"};

    menu = new Gui::PopMenu(0, 0, 100, 200, nullptr);
    menu->Visible(true);
    menu->AddItems(names);
    g_manager->Add(menu);

    l->AddItems(names);   
    g_manager->Add(l);
    Gui::ComboBox* b = new Gui::ComboBox(0, 0, 120, 200, nullptr);
    b->AddItems(names);
    g_manager->Add(b);
    CreateWidget();
}

int main(int argc, char *argv[])
{
    d = new Dialog;
    // Get HGE interface
    hge = hgeCreate(HGE_VERSION);

    // Set up log file, frame function, render function and window title
    hge->System_SetState(HGE_LOGFILE, "hge_tut02.log");
    hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
    hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
    hge->System_SetState(HGE_TITLE, "HGE Tutorial 02 - Using input, sound and rendering");

    // Set up video mode
    hge->System_SetState(HGE_WINDOWED, true);
    hge->System_SetState(HGE_SCREENWIDTH, 800);
    hge->System_SetState(HGE_SCREENHEIGHT, 600);
    hge->System_SetState(HGE_SCREENBPP, 32);

    if(hge->System_Initiate())
    {
        InitGui();
        // Load sound and texture
        snd=hge->Effect_Load("../data/menu.ogg");
        quad.tex=hge->Texture_Load("../data/particles.png");
        if(!snd || !quad.tex)
        {
            fprintf(stderr, "Error: Can't load menu.wav or particles.png\n");
            hge->System_Shutdown();
            hge->Release();
            return 0;
        }

        // Set up quad which we will use for rendering sprite
        quad.blend=BLEND_ALPHAADD | BLEND_COLORMUL | BLEND_ZWRITE;

        for(int i=0;i<4;i++)
        {
            // Set up z-coordinate of vertices
            quad.v[i].z=0.5f;
            // Set up color. The format of DWORD col is 0xAARRGGBB
            quad.v[i].col=0xFFFFA000;
        }

        // Set up quad's texture coordinates.
        // 0,0 means top left corner and 1,1 -
        // bottom right corner of the texture.
        quad.v[0].tx=96.0/128.0; quad.v[0].ty=64.0/128.0;
        quad.v[1].tx=128.0/128.0; quad.v[1].ty=64.0/128.0;
        quad.v[2].tx=128.0/128.0; quad.v[2].ty=96.0/128.0;
        quad.v[3].tx=96.0/128.0; quad.v[3].ty=96.0/128.0;

        // Let's rock now!

        hge->System_Start();

        // Free loaded texture and sound
        hge->Texture_Free(quad.tex);
        hge->Effect_Free(snd);
        delete g_manager;
    }
    else {
        fprintf(stderr, "Error: %s\n", hge->System_GetErrorMessage());
    }

    // Clean up and shutdown
    hge->System_Shutdown();
    hge->Release();
    return 0;
}
