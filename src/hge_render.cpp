#include <hge.h>
#include <hgefont.h>
#include "hge_render.h"
#include "gui_manager.h"

namespace Gui
{    
    DWORD FloatToDword(float r, float g, float b, float a)
    {
        DWORD rr = (DWORD)(r * 255.0f);
        DWORD gg = (DWORD)(g * 255.0f);
        DWORD bb = (DWORD)(b * 255.0f);
        DWORD aa = (DWORD)(a * 255.0f);
        DWORD c = aa << 24 | rr << 16 | gg << 8 | bb;
        return c;
    }

    HgeRender::HgeRender(Manager *value)
        : m_manager(value)
    {
        cs = new RenderState;        
    }

    HgeRender::~HgeRender()
    {

    }

    void HgeRender::DrawQuad(float x, float y, float width, float height)
    {
        hgeQuad quad;
        quad.v[0].x = x + cs->x;
        quad.v[0].y = y + cs->y;
        quad.v[1].x = x + width + cs->x;
        quad.v[1].y = y + cs->y;
        quad.v[2].x = x + width + cs->x;
        quad.v[2].y = y + height + cs->y;
        quad.v[3].x = x + cs->x;
        quad.v[3].y = y + height + cs->y;

        for( int i = 0; i < 4; i++ )
        {
            quad.v[i].z = 0.5f;
            quad.v[i].col = FloatToDword(cs->r, cs->g, cs->b, cs->a);
        }

        quad.tex = 0;
        quad.blend = BLEND_DEFAULT;
        m_manager->GetHge()->Gfx_RenderQuad(&quad);
    }

    void HgeRender::PushSate()
    {
        m_state.push(cs);
        cs = new RenderState(*cs);
    }

    void HgeRender::PopState()
    {
        if (m_state.empty())
            return;
        delete cs;
        cs = m_state.top();
        m_state.pop();
    }

    void HgeRender::Translate(float dx, float dy)
    {
        cs->x += dx;
        cs->y += dy;
    }

    void HgeRender::SetPosition(float x, float y)
    {
        cs->x = x;
        cs->y = y;
    }

    void HgeRender::Rotate(float da)
    {
        cs->angle += da;
    }

    void HgeRender::SetRotation(float a)
    {
        cs->angle = a;
    }

    void HgeRender::DrawPoint(float x, float y)
    {
        //  TODO: Lock texture for more efficiency
        DrawQuad(x, y, 1, 1);
    }

    void HgeRender::DrawLine(float x1, float y1, float x2, float y2)
    {
        m_manager->GetHge()->Gfx_RenderLine(x1 + cs->x, y1 + cs->y, x2 + cs->x, y2 + cs->y, FloatToDword(cs->r, cs->g, cs->b, cs->a));
    }

    void HgeRender::DrawTextLine(float x, float y, const std::string& s)
    {
        if (m_manager->GetHgeFont())
        {
            m_manager->GetHgeFont()->SetColor(FloatToDword(cs->r, cs->g, cs->b, cs->a));
            m_manager->GetHgeFont()->SetScale(0.8);
            m_manager->GetHgeFont()->Render(x+cs->x, y+cs->y, HGETEXT_LEFT, s.c_str());
        }
    }

    void HgeRender::SetColor(float r, float g, float b)
    {
        cs->r = r; cs->g = g; cs->b = b;
    }

    void HgeRender::SetAlpha(float a)
    {
        cs->a = a;
    }        
}
