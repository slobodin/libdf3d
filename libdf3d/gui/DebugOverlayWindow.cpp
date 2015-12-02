#include "DebugOverlayWindow.h"

#include "GuiManager.h"
#include <base/EngineController.h>
#include <render/RenderStats.h>
#include <scene/SceneManager.h>
#include <scene/Node.h>
#include <scene/Camera.h>
#include <scene/SceneManagerListener.h>
#include <scene/Scene.h>
#include <utils/Utils.h>
#include <resources/FileSystem.h>
#include <resources/FileDataSource.h>
#include <components/MeshComponent.h>
#include <components/LightComponent.h>
#include <components/ParticleSystemComponent.h>
#include <physics/PhysicsManager.h>

namespace df3d {

//namespace impl
//{
//
//void SetRmlAndStyle(Rocket::Core::ElementDocument *document, const char *rml, const char *rcss)
//{
//    // Just for test:
//    // TODO:
//    // Embed rml and rcss.
//    auto file = svc().fileSystem().openFile(rml);
//    std::string buffer(file->getSize(), 0);
//    file->getRaw(&buffer[0], file->getSize());
//
//    document->SetInnerRML(buffer.c_str());
//
//    file = svc().fileSystem().openFile(rcss);
//    buffer.resize(file->getSize());
//    file->getRaw(&buffer[0], file->getSize());
//
//    Rocket::Core::StyleSheet* styleSheet = Rocket::Core::Factory::InstanceStyleSheetString(Rocket::Core::String(buffer.c_str()));
//
//    document->SetStyleSheet(styleSheet);
//    styleSheet->RemoveReference();
//}
//
//class StatsWindow : public Rocket::Core::ElementDocument
//{
//    RocketElement m_trianglesValue, m_cameraPositionValue,
//        m_cameraRotationValue, m_lightsValue, m_nodesValue, m_psValue, m_particlesValue,
//        m_collisionObjsValue;
//
//    void OnRender()
//    {
//        auto stats = svc().getLastRenderStats();
//
//        std::string triangles = "Triangles: ";
//        triangles += utils::to_string(stats.totalTriangles);
//
//        std::string totalLights = "Lights: ";
//        totalLights += utils::to_string(stats.totalLights);
//
//        std::string totalNodes = "Mesh Nodes: ";
//        totalNodes += utils::to_string(stats.totalNodes);
//
//        std::string psNodes = "Particle systems: ";
//        psNodes += utils::to_string(stats.totalParticleSystems);
//
//        std::string particles = "Particles: ";
//        particles += utils::to_string(stats.totalParticles);
//
//        m_trianglesValue->SetInnerRML(triangles.c_str());
//        m_lightsValue->SetInnerRML(totalLights.c_str());
//        m_nodesValue->SetInnerRML(totalNodes.c_str());
//        m_psValue->SetInnerRML(psNodes.c_str());
//        m_particlesValue->SetInnerRML(particles.c_str());
//
//        auto cam = svc().sceneManager().getCamera();
//        if (cam)
//        {
//            auto camPos = cam->getPosition();
//            std::string camPosStr = "Camera position: ";
//            camPosStr += df3d::glmVecToString(camPos);
//            m_cameraPositionValue->SetInnerRML(camPosStr.c_str());
//
//            auto camRot = cam->getRotation();
//            std::string camRotStr = "Camera rotation: ";
//            camRotStr += df3d::glmVecToString(camRot);
//            m_cameraRotationValue->SetInnerRML(camRotStr.c_str());
//        }
//
//        std::string collObjs = "BP collision objs: ";
//        collObjs += utils::to_string(g_physicsWorld->getNumCollisionObjects());
//
//        m_collisionObjsValue->SetInnerRML(collObjs.c_str());
//    }
//
//public:
//    StatsWindow(const Rocket::Core::String& tag)
//        : ElementDocument(tag)
//    {
//        SetRmlAndStyle(this, "gui/debug_wnd/debug_stats.rml", "gui/debug_wnd/debug_stats.rcss");
//
//        m_trianglesValue = GetElementById("triangles-value");
//        m_cameraPositionValue = GetElementById("camera-position");
//        m_cameraRotationValue = GetElementById("camera-rotation");
//        m_lightsValue = GetElementById("lights-value");
//        m_nodesValue = GetElementById("nodes-value");
//        m_psValue = GetElementById("ps-value");
//        m_particlesValue = GetElementById("particles-value");
//        m_collisionObjsValue = GetElementById("collision-objs");
//    }
//};
//
//class ConsoleWindow : public Rocket::Core::ElementDocument, public Rocket::Core::EventListener
//{
//    RocketElement m_inputText;
//    RocketElement m_historyElement;
//
//    std::list<Rocket::Core::String> m_history;
//    Rocket::Core::String m_historyStr;
//
//    void ProcessEvent(Rocket::Core::Event &ev)
//    {
//        if (ev == "click")
//        {
//            if (ev.GetTargetElement()->GetId() == "submit_command")
//            {
//                submitConsoleCommand();
//            }
//        }
//    }
//
//public:
//    DebugOverlayWindow *m_debugWnd = nullptr;
//
//    ConsoleWindow(const Rocket::Core::String& tag)
//        : ElementDocument(tag)
//    {
//        SetRmlAndStyle(this, "gui/debug_wnd/debug_console.rml", "gui/debug_wnd/debug_console.rcss");
//
//        m_inputText = GetElementById("console_input");
//        m_historyElement = GetElementById("previous_commands");
//    }
//
//    void updateHistory(const Rocket::Core::String &command, const Rocket::Core::String &result)
//    {
//        if (command.Empty())
//            return;
//
//        m_history.push_back(command);
//
//        if (!m_historyStr.Empty())
//            m_historyStr += "\n";
//
//        m_historyStr += command + " -> " + result;
//
//        m_historyElement->SetInnerRML(m_historyStr);
//    }
//
//    void submitConsoleCommand()
//    {
//        auto command = m_inputText->GetAttribute<Rocket::Core::String>("value", "");
//
//        std::string result;
//        m_debugWnd->onCommandInvoked(command.CString(), result);
//
//        updateHistory(command, result.c_str());
//    }
//};
//
//class SceneTreeWindow : public Rocket::Core::ElementDocument, public SceneManagerListener, public Rocket::Core::EventListener
//{
//public:
//    const Node *m_currentNode = nullptr;
//    std::string m_currentNodeName;
//    const scene::Scene *m_scene = nullptr;
//
//    const char *getNodeType(const Node *n)
//    {
//        //if (dynamic_cast<const scene::Scene *>(n))
//        //    return "Scene";
//        //else if (dynamic_cast<const scene::MeshNode *>(n))
//        //    return "MeshNode";
//        //else if (dynamic_cast<const particlesys::ParticleSystemNode *>(n))
//        //    return "ParticleSystem";
//        //else if (dynamic_cast<const scene::LightNode *>(n))
//        //    return "LightNode";
//
//        return "UnknownNodeType";
//    }
//
//    SceneTreeWindow(const Rocket::Core::String& tag)
//        : ElementDocument(tag)
//    {
//        Rocket::Core::StyleSheet* styleSheet = Rocket::Core::Factory::InstanceStyleSheetFile(Rocket::Core::String("gui/debug_wnd/debug_scene_tree.rcss"));
//
//        SetStyleSheet(styleSheet);
//        styleSheet->RemoveReference();
//    }
//
//    void onSceneCleared()
//    {
//        m_currentNode = nullptr;
//        m_scene = nullptr;
//        m_currentNodeName = "";
//
//        SetInnerRML("");
//    }
//
//    void onSceneCreated(const scene::Scene *sc)
//    {
//        m_scene = sc;
//        recreateTree(sc->getRoot().get());
//    }
//
//    void onNodeAddedToScene(const Node *node)
//    {
//        recreateTree(m_currentNode);
//    }
//
//    void onNodeRemovedFromScene()
//    {
//        // FIXME:
//        // Check for node being removed.
//        recreateTree(m_scene->getRoot().get());
//    }
//
//    void recreateTree(const Node *node)
//    {
//        if (!node)
//            return;
//
//        m_currentNode = node;
//        m_currentNodeName = node->getName();
//
//        if (!IsVisible())
//            return;
//
//        Rocket::Core::String rml = "<div>";
//
//        rml += "<p class='header'>Node name: ";
//        rml += m_currentNodeName.c_str();
//        rml += "</p>";
//        rml += "<p>Type: ";
//        rml += getNodeType(m_currentNode);
//        rml += "</p>";
//        rml += "<button id='back'>..</button>";
//
//        for (auto it = node->cbegin(); it != node->cend(); it++)
//        {
//            rml += "<button id='";
//            rml += it->second->getName().c_str();
//            rml += "'>";
//            rml += it->second->getName().c_str();
//            rml += "</button>";
//        }
//
//        rml += "</div>";
//
//        rml += "<div><p class='header'>Node properties</p>";
//        rml += createVector("position", glm::vec3());
//        rml += createVector("rotation (euler, degrees)", glm::vec3());
//        rml += createVector("scale", glm::vec3());
//        rml += "</div>";
//
//        SetInnerRML(rml);
//
//        for (auto it = node->cbegin(); it != node->cend(); it++)
//            GetElementById(it->second->getName().c_str())->AddEventListener("click", this);
//    }
//
//    Rocket::Core::String createVector(const char *name, const glm::vec3 &vals)
//    {
//        Rocket::Core::String result;
//
//        result += "<p>";
//        result += name;
//        result += "</p>";
//        result += "<input type='text' class='vector_comp' maxlength='10'></input>";
//        result += "<input type='text' class='vector_comp' maxlength='10'></input>";
//        result += "<input type='text' class='vector_comp' maxlength='10'></input>";
//
//        return result;
//    }
//
//    void showProperties(const Node *node)
//    {
//
//    }
//
//    void ProcessEvent(Rocket::Core::Event &ev)
//    {
//        if (!(ev == "click"))
//            return;
//
//        auto name = ev.GetTargetElement()->GetId();
//        if (name == m_currentNode->getName().c_str())
//            return;
//
//        Node *newNode = nullptr;
//
//        if (name == "back")
//            newNode = m_currentNode->getParent().get();
//        else
//            newNode = m_currentNode->getChildByName(name.CString()).get();
//
//        recreateTree(newNode);
//    }
//};
//
//}
//
//void DebugOverlayWindow::ProcessEvent(Rocket::Core::Event &ev)
//{
//    if (ev == "click")
//    {
//        if (ev.GetTargetElement()->GetId() == "show_stats_button")
//        {
//            m_statsWnd->SetProperty("visibility", m_statsWnd->IsVisible() ? "hidden" : "visible");
//            m_consoleWnd->SetProperty("visibility", "hidden");
//            m_sceneTreeWnd->SetProperty("visibility", "hidden");
//        }
//        else if (ev.GetTargetElement()->GetId() == "open_console_button")
//        {
//            m_consoleWnd->SetProperty("visibility", m_consoleWnd->IsVisible() ? "hidden" : "visible");
//            m_statsWnd->SetProperty("visibility", "hidden");
//            m_sceneTreeWnd->SetProperty("visibility", "hidden");
//        }
//        else if (ev.GetTargetElement()->GetId() == "open_scene_tree")
//        {
//            m_sceneTreeWnd->SetProperty("visibility", m_sceneTreeWnd->IsVisible() ? "hidden" : "visible");
//            m_statsWnd->SetProperty("visibility", "hidden");
//            m_consoleWnd->SetProperty("visibility", "hidden");
//
//            if (m_sceneTreeWnd->IsVisible())
//                m_sceneTreeWnd->recreateTree(m_sceneTreeWnd->m_currentNode);
//        }
//    }
//}
//
//void DebugOverlayWindow::createDebugMenu(Rocket::Core::Context *context)
//{
//    m_debugMenu = context->CreateDocument();
//
//    m_debugMenu->SetId("__debug_menu");
//    m_debugMenu->SetProperty("visibility", "hidden");
//
//    impl::SetRmlAndStyle(m_debugMenu, "gui/debug_wnd/debug_window.rml", "gui/debug_wnd/debug_window.rcss");
//
//    m_debugMenu->GetElementById("show_stats_button")->AddEventListener("click", this);
//    m_debugMenu->GetElementById("open_console_button")->AddEventListener("click", this);
//    m_debugMenu->GetElementById("open_scene_tree")->AddEventListener("click", this);
//}
//
//void DebugOverlayWindow::createDebugStatsWindow(Rocket::Core::Context *context)
//{
//    Rocket::Core::Factory::RegisterElementInstancer("__debug_stats_window", new Rocket::Core::ElementInstancerGeneric<impl::StatsWindow>())->RemoveReference();
//    m_statsWnd = dynamic_cast<impl::StatsWindow *>(context->CreateDocument("__debug_stats_window"));
//
//    m_statsWnd->SetProperty("visibility", "hidden");
//}
//
//void DebugOverlayWindow::createConsoleWindow(Rocket::Core::Context *context)
//{
//    Rocket::Core::Factory::RegisterElementInstancer("__debug_console_window", new Rocket::Core::ElementInstancerGeneric<impl::ConsoleWindow>())->RemoveReference();
//    m_consoleWnd = dynamic_cast<impl::ConsoleWindow *>(context->CreateDocument("__debug_console_window"));
//
//    m_consoleWnd->SetProperty("visibility", "hidden");
//    m_consoleWnd->m_debugWnd = this;
//}
//
//void DebugOverlayWindow::createSceneTreeWindow(Rocket::Core::Context *context)
//{
//    Rocket::Core::Factory::RegisterElementInstancer("__debug_scene_tree_window", new Rocket::Core::ElementInstancerGeneric<impl::SceneTreeWindow>())->RemoveReference();
//    m_sceneTreeWnd = dynamic_cast<impl::SceneTreeWindow *>(context->CreateDocument("__debug_scene_tree_window"));
//
//    svc().sceneManager().registerListener(m_sceneTreeWnd);
//
//    m_sceneTreeWnd->SetProperty("visibility", "hidden");
//}
//
//void DebugOverlayWindow::onCommandInvoked(const std::string &command, std::string &result)
//{
//    svc().consoleCommandInvoked(command, result);
//}
//
//DebugOverlayWindow::DebugOverlayWindow()
//{
//    Rocket::Core::Context *context = svc().guiManager().getRocketContext();
//
//    createDebugMenu(context);
//    createDebugStatsWindow(context);
//    createConsoleWindow(context);
//    createSceneTreeWindow(context);
//}
//
//DebugOverlayWindow::~DebugOverlayWindow()
//{
//}
//
//void DebugOverlayWindow::toggle()
//{
//    m_visible = !m_visible;
//
//    m_debugMenu->SetProperty("visibility", m_visible ? "visible" : "hidden");
//
//    if (!m_visible)
//    {
//        m_statsWnd->SetProperty("visibility", "hidden");
//        m_consoleWnd->SetProperty("visibility", "hidden");
//        m_sceneTreeWnd->SetProperty("visibility", "hidden");
//    }
//}
//
//void DebugOverlayWindow::onRocketShutdown()
//{
//    svc().sceneManager().unregisterListener(m_sceneTreeWnd);
//
//    if (m_debugMenu)
//        m_debugMenu->RemoveReference();
//    if (m_statsWnd)
//        m_statsWnd->RemoveReference();
//    if (m_consoleWnd)
//        m_consoleWnd->RemoveReference();
//    if (m_sceneTreeWnd)
//        m_sceneTreeWnd->RemoveReference();
//
//    m_debugMenu = nullptr;
//    m_statsWnd = nullptr;
//    m_consoleWnd = nullptr;
//    m_sceneTreeWnd = nullptr;
//}
//
//void DebugOverlayWindow::onMouseButtonEvent(const SDL_MouseButtonEvent &mouseButtonEvent)
//{
//    if (mouseButtonEvent.button != SDL_BUTTON_LEFT || mouseButtonEvent.type != SDL_MOUSEBUTTONDOWN)
//        return;
//
//    //if (!m_sceneTreeWnd->IsVisible())
//    //    return;
//}

}
