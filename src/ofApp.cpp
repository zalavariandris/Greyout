#include "ofApp.hpp"
#include "ofDrawUtilities.hpp"
#include "ImGuiUtilities.hpp"
#include "imgui_tabs.h"

template<class T>
int remainder(T a, T b){
    return (b + (a%b)) % b;
}


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowTitle("Greyout");
    font.load("verdana.ttf", 36, true, true, true);
    gui.setup();
    ofSetVerticalSync(true);
    ofDisableArbTex();
    camera = make_shared<ofEye>();
    camera->setup();
}

//--------------------------------------------------------------
void ofApp::update(){
    camera->update();
}

void ofApp::step_GACamera_coroutine(coroutine<void>::pull_type &VSYNC){
    int CAPTURE_LATENCY {3};
    /* Init cadidates */
    pop.init(9);
    VSYNC();
    
    while(true){
        /* GA loop */
        for(auto i=0; i<pop.size()+CAPTURE_LATENCY; i++){
            /* Pair phenotypes and captures */
            if(i < pop.size()){
                // create phenotype
                auto candidate = pop.candidate(i);
                ofVec2f size = rgb_size(candidate->genes->length(), 3);
                candidate->image = make_image(size, [&](int w, int h){
                    ofSetColor(255, 255, 255);
                    draw_bw(candidate->genes->sequence(), candidate->genes->length(), 0, 0, w, h);
                });
                candidate->image->draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
            }
            if(i >= CAPTURE_LATENCY){
                
                // capture image
                auto candidate = pop.candidate(i-CAPTURE_LATENCY);
                ofVec2f size = rgb_size(candidate->genes->length(), 3);
                candidate->capture = make_image(size, [&](int w, int h){
                    camera->buffer->draw(0, 0, w, h);
                });
                
                /* Evaluate cost */
                candidate->cost = compare_lightness(*candidate->image, *candidate->capture);
            }
            VSYNC();
        }
        
        // keep a copy of the best candidate
        best_candidate = pop.best();
        VSYNC();
        
        /* Breed new generation */
        pop.breed();
    }
}

void ofApp::camera_sync_test(){
    static int latency{4};
    static deque<shared_ptr<ofImage>> images(10);
    static deque<shared_ptr<ofImage>> captures(10);
    static int k=0;
    
    ImGui::Begin("");
    shared_ptr<ofImage> image = make_image(ofVec2f(320, 240), [&](int w, int h){
        ofPushMatrix();
        string text = ofToString(remainder(k, 10));
        ofRectangle rect = font.getStringBoundingBox(text, 0, 0);
        ofScale(w/rect.width, h/rect.height);
        ofDrawMeshString(font, text, 0, font.getSize(), 0);
        ofPopMatrix();
    });
    ImGui::Image((void *)(intptr_t) (image && image->isAllocated() ? image->getTexture().getTextureData().textureID : 0), ImVec2(image->getWidth(), image->getHeight()));
    ImGui::End();
    images[remainder(k, 10)] = image;
    
    shared_ptr<ofImage> capture = make_image(ofVec2f(320, 240), [&](int w, int h){
        camera->buffer->draw(0,0);
    });
    
    captures[remainder(k-latency, 10)] = capture;
    
    ImGui::InputInt("latency", &latency);
    ImGui::BeginGroup();
    for(auto i=0; i<images.size(); i++) //captures[i]->draw(40, i*40, 40, 40);
        ImGui::Image((void *)(intptr_t) (images[i] && images[i]->isAllocated() ? images[i]->getTexture().getTextureData().textureID : 0), ImVec2(40, 40));
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();
    
    for(auto i=0; i<captures.size(); i++) //captures[i]->draw(40, i*40, 40, 40);
        ImGui::Image((void *)(intptr_t) (captures[i] && captures[i]->isAllocated() ? captures[i]->getTexture().getTextureData().textureID : 0), ImVec2(40, 40));
    ImGui::EndGroup();
        
    k++;
}

void ofApp::step_GAImage_coroutine(coroutine<void>::pull_type &VSYNC){
    /* Init cadidates */
    pop.init(9);
    VSYNC();
    
    while(true){
        /* GA loop */
        
        // decode phenotype
        for(auto candidate : pop.candidates){
            // create phenotype
            ofVec2f size = rgb_size(candidate->genes->length(), 3);
            candidate->image = make_image(size, [&](int w, int h){
                ofSetColor(255, 255, 255);
                draw_rgb(candidate->genes->sequence(), candidate->genes->length(), 0, 0, w, h);
            });
        }
        
        // Evaluate
        for(auto candidate : pop.candidates){
            ofVec2f size = rgb_size(candidate->genes->length(), 3);
            
            candidate->capture = make_image(size, [&](int w, int h){
                camera->buffer->draw(0, 0, w, h);
            });;
            candidate->cost = compare_images(*candidate->image, *candidate->capture);
        }
        // keep a copy of the best candidate
        best_candidate = pop.best();
        VSYNC();
        
        /* Breed new generation */
        pop.breed();
    }
}

struct Signal{
    int k;
    vector<float> values;
    Signal(int max_size) : values(max_size), k(0){}
};

namespace ImGui{
    void Image(shared_ptr<ofImage> image, const ImVec2 &size){
        ImGui::Image((void *)(intptr_t) (image->isAllocated() ? image->getTexture().getTextureData().textureID : 0), size);
    }
    void Image(shared_ptr<ofImage> image){
        ImGui::Image(image, ImVec2(image->getWidth(), image->getHeight()));
    }
    
//    PlotLines(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0,0), int stride = sizeof(float));
    void PlotSignal(const char *label, const float value, int max_size=0, bool scroll=false, const char* overlay_text=NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0,0), int stride = sizeof(float)){
        static vector<float> values(max_size);
        static int k=0;
        static Signal signal(10);
        
        if(max_size)
            values[remainder(k, max_size)] = value;
        else
            values.push_back(value);
    
        ImGui::PlotLines("", values.data(), max_size ? max_size : values.size(), scroll ? k : 0, overlay_text, scale_min, scale_max, graph_size, stride);
        k++;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    gui.begin();
    static int algorithm{0};
    ImGui::Text("algorithm");
    ImGui::RadioButton("pause", &algorithm, 0);
    ImGui::RadioButton("image", &algorithm, 1);
    ImGui::RadioButton("projection", &algorithm, 2);
    
    static bool play{true};
    switch (algorithm) {
        case 1:
            step_GAImage();
            break;
        case 2:
            step_GACamera();
            break;
        default:
            break;
    }
    
    /* GUI */
    onGui();
    gui.end();
}

void ofApp::onGui(){
    ImGui::Begin("Greyout", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar);
    static vector<float> fpsv(50);
    fpsv[0] = 1.0/ofGetLastFrameTime();
    std::rotate(fpsv.begin(), fpsv.begin()+1, fpsv.end());
    ImGui::PlotLines ("", fpsv.data(), fpsv.size(), 0, "", 0, 128);
    
    static bool eye_window{false};
    static bool candidates_window{true};
    static bool statistics_window{true};
    static bool profiler_window{false};
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Windows"))
        {
            ImGui::MenuItem("Eye", "Ctrl+1", &eye_window);
            ImGui::MenuItem("Candidates", "Ctrl+2", &candidates_window);
            ImGui::MenuItem("Statistics", "Ctrl+3", &statistics_window);
            ImGui::MenuItem("Profiler", "Ctrl+4", &profiler_window);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    
    if (ImGui::CollapsingHeader("Eye"))
        ImGui::Eye(*camera);
    if (ImGui::CollapsingHeader("GA")){
        ImGui::Text("population size: %lu", pop.size());
        ImGui::Text("mutation rate: ");
        ImGui::Text("crossover rate: ");
        static int mutation_type{0};
        ImGui::Text("mutation type");
        ImGui::RadioButton("uniform", &mutation_type, 0); ImGui::SameLine();
        ImGui::RadioButton("tweak", &mutation_type, 1);
        static int crossover_type{0};
        ImGui::Text("crossover type");
        ImGui::RadioButton("uniform_crossover", &crossover_type, 0);
        ImGui::Text("Chromosome length: %lu", pop.size()>0 ? pop.candidate(0)->genes->length() : -1);
        static int parse_method{0};
        ImGui::Text("parse method");
        ImGui::RadioButton("rgb", &parse_method, 0); ImGui::SameLine();
        ImGui::RadioButton("b&w", &parse_method, 1);
    }
    if(eye_window){
        ImGui::Begin("Eye", &eye_window, ImGuiWindowFlags_NoCollapse);
        ImGui::Image(camera->buffer, ImGui::GetContentRegionAvail());
        ImGui::End();
    }
    if(candidates_window){
        ImGui::Begin("Candidates", &candidates_window, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
        ImVec2 size(20, 20);
        for(auto candidate : pop){
            ImGui::Image((void *)(intptr_t) (candidate->image ? candidate->image->getTexture().getTextureData().textureID : 0), size);
            ImGui::SameLine();
            ImGui::Image((void *)(intptr_t) (candidate->capture ? candidate->capture->getTexture().getTextureData().textureID : 0), size);
            ImGui::SameLine();
            
            ImGui::Text("%6.3f", candidate->cost);
            if(ImGui::GetItemRectMax().x - ImGui::GetWindowPos().x + size.x*2 <= ImGui::GetContentRegionAvailWidth())
                ImGui::SameLine();
        }
        ImGui::End();
    }
    if(statistics_window){
        ImGui::Begin("Statistics", &statistics_window, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Population size: %lu", pop.size());
        ImGui::Text("Generation: %i", pop.generation);
        if(pop.cost_history.size()>0){
            ImGui::Text("Current best: %6.3f", pop.cost_history[pop.cost_history.size()-1]);
            ImVec2 size = ImGui::GetContentRegionAvail();
            
            static vector<float> bestv(0);
            bestv.push_back(pop.best()->cost);
            ImGui::PlotLines("", bestv.data(), bestv.size(), 0, "", 0, 99/*max cost*/, ImVec2(256, 128));
//            ImGui::PlotSignal("", pop.best()->cost, 0, true, "", 0, 99/*max cost*/, ImVec2(256, 128));
//            static vector<float> values(10);
//            std::rotate(values.begin(), values.begin() + 1, values.end());
//            values[0] = pop.best()->cost;
//            ImGui::PlotLines("", values.data(), values.size(), 0, "", 0, 128, ImVec2(256, 128));
        }
        ImGui::End();
    }

//        if(ImGui::TabItem("Sync"))
//            camera_sync_test();

    ImGui::End();
}

