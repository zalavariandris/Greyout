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

void ofApp::decode(shared_ptr<GA::Candidate> candidate){
    // create phenotype
    if(decodeMethod == DecodeColor){
        ofVec2f size = rgb_size(candidate->genes->length(), 3);
        candidate->image = make_image(size, [&](int w, int h){
            ofSetColor(255, 255, 255);
            draw_rgb(candidate->genes->sequence(), candidate->genes->length(), 0, 0, w, h);
        });
    }else if(decodeMethod == DecodeGreyscale){
        ofVec2f size = bw_size(candidate->genes->length());
        candidate->image = make_image(size, [&](int w, int h){
            ofSetColor(255, 255, 255);
            draw_bw(candidate->genes->sequence(), candidate->genes->length(), 0, 0, w, h);
        });
    }else if(decodeMethod == DecodeIndex){
        candidate->image = make_image(ofVec2f(128, 128), [&](int w, int h){
            ofPushMatrix();
            auto it = std::find(pop.candidates.begin(), pop.candidates.end(), candidate);
            auto idx = std::distance(pop.begin(), it);
            string text = ofToString(idx);
            ofRectangle rect = font.getStringBoundingBox(text, 0, 0);
            ofScale(w/rect.width, h/rect.height);
            ofDrawMeshString(font, text, 0, font.getSize(), 0);
            ofPopMatrix();
        });
    }
}

void ofApp::evaluate(shared_ptr<GA::Candidate> candidate, shared_ptr<ofImage> environment){
    if(evaluateMethod == EvaluateCompareColor){
        ofVec2f size = rgb_size(candidate->genes->length(), 3);
        candidate->capture = make_image(size, [&](int w, int h){
            environment->draw(0,0,w,h);
        });;
        candidate->cost = compare_images(*candidate->image, *candidate->capture);
    }else if(evaluateMethod == EvaluateCompareGreyscale){
        ofVec2f size = bw_size(candidate->genes->length());
        candidate->capture = make_image(size, [&](int w, int h){
            environment->draw(0,0,w,h);
        });;
        candidate->cost = compare_brightness(*candidate->image, *candidate->capture);
    }else if(evaluateMethod == EvaluateCaptureGreyness){
        candidate->cost = std::numeric_limits<float>::infinity();
    }
}

void ofApp::step_GACamera_coroutine(coroutine<void>::pull_type &VSYNC){
    /* Init cadidates */
    pop.init(20);
    VSYNC();
    
    while(true){
        /* GA loop */
        for(auto i=0; i<pop.size()+cameraLatency; i++){
            /* Pair phenotypes and captures */
            if(i < pop.size()){
                // create phenotype
                decode(pop.candidate(i));
                pop.candidate(i)->image->draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
            }
            if(i >= cameraLatency){
                shared_ptr<ofImage> capture = camera->buffer;
                evaluate(pop.candidate(i-cameraLatency), capture);
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
    pop.init(20);
    VSYNC();
    
    while(true){
        /* GA loop */
        
        // decode phenotype
        for(auto candidate : pop.candidates)
            decode(candidate);
        
        // Evaluate
        for(auto candidate : pop.candidates){
            shared_ptr<ofImage> capture = camera->buffer;
            evaluate(candidate, capture);
        }
        
        // keep a copy of the best candidate
        best_candidate = pop.best();
        VSYNC();
        
        /* Breed new generation */
        pop.breed();
    }
}

namespace ImGui{
    void Image(shared_ptr<ofImage> image, const ImVec2 &size){
        ImGui::Image((void *)(intptr_t) (image->isAllocated() ? image->getTexture().getTextureData().textureID : 0), size);
    }
    void Image(shared_ptr<ofImage> image){
        ImGui::Image(image, ImVec2(image->getWidth(), image->getHeight()));
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    static int algorithm{0};
    switch (algorithm) {
        case 0:
            /* do nothing */
            break;
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
    gui.begin();
    ImGui::Text("algorithm");
    ImGui::RadioButton("pause", &algorithm, 0);
    ImGui::RadioButton("image", &algorithm, 1);
    ImGui::RadioButton("projection", &algorithm, 2);
    onGui();
    gui.end();
}

void ofApp::onGui(){
    ImGui::Begin("Greyout", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar);
    static vector<float> fpsv(50);
    fpsv[0] = 1.0/ofGetLastFrameTime();
    std::rotate(fpsv.begin(), fpsv.begin()+1, fpsv.end());
    ImGui::PlotLines ("", fpsv.data(), fpsv.size(), 0, "", 0, 128);
    
    static bool eye_window{true};
    static bool candidates_window{true};
    static bool statistics_window{true};
    static bool profiler_window{true};
    
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
    
    ImGui::InputInt("camera latency", &cameraLatency);
    
    if (ImGui::CollapsingHeader("Eye"))
        ImGui::Eye(*camera);
    
    if (ImGui::CollapsingHeader("GA")){
        if(ImGui::Button("Reset"))
            pop.init(20);
        
        ImGui::Text("Population Size: %lu", pop.size());
        ImGui::Text("Mutation Rate: ?");
        ImGui::Text("Crossover Rate: ?");
        ImGui::Text("Mutation Method:"); ImGui::SameLine();
        ImGui::RadioButton("uniform", &mutationMethod, MutationUniform); ImGui::SameLine();
        ImGui::RadioButton("tweak", &mutationMethod, MutationTweak);
        ImGui::Text("Crossover Method:"); ImGui::SameLine();
        ImGui::RadioButton("uniform", &crossoverMethod, CrossoverUniform);
        ImGui::Text("Chromosome length: %lu", pop.size()>0 ? pop.candidate(0)->genes->length() : 0);
        ImGui::Text("Decode Method:"); ImGui::SameLine();
        ImGui::RadioButton("rgb", &decodeMethod, DecodeColor); ImGui::SameLine();
        ImGui::RadioButton("b&w", &decodeMethod, DecodeGreyscale); ImGui::SameLine();
        ImGui::RadioButton("idx", &decodeMethod, DecodeIndex);
        ImGui::Text("Evaluation Method:");
        ImGui::RadioButton("Compare colors", &evaluateMethod, EvaluateCompareColor);
        ImGui::RadioButton("Compare brightness", &evaluateMethod, EvaluateCompareGreyscale);
        ImGui::RadioButton("Campture greyness", &evaluateMethod, EvaluateCaptureGreyness);
    }
    
    if(eye_window){
        ImGui::Begin("Eye", &eye_window);
        ImGui::Image(camera->buffer, ImGui::GetContentRegionAvail());
        ImGui::End();
    }
    
    if(candidates_window){
        ImGui::Begin("Candidates", &candidates_window);
        static int folder_view{0};
        static int size{40};
        ImGui::RadioButton("details", &folder_view, 0); ImGui::SameLine();
        ImGui::RadioButton("icons", &folder_view, 1); ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::SliderInt("size", &size, 5, 200);
        if(folder_view==0)
            for(auto candidate : pop){
                ImGui::Image((void *)(intptr_t) (candidate->image ? candidate->image->getTexture().getTextureData().textureID : 0), ImVec2(size, size));
                ImGui::SameLine();
                ImGui::Image((void *)(intptr_t) (candidate->capture ? candidate->capture->getTexture().getTextureData().textureID : 0), ImVec2(size, size));
                ImGui::SameLine();
                ImGui::Text("%6.3f", candidate->cost);
            }
        
        else if(folder_view==1)
            for(auto candidate : pop){
                ImGui::Image((void *)(intptr_t) (candidate->image ? candidate->image->getTexture().getTextureData().textureID : 0), ImVec2(size, size));
                
                if(ImGui::GetItemRectMax().x - ImGui::GetWindowPos().x + size <= ImGui::GetContentRegionAvailWidth())
                    ImGui::SameLine();
            }
            
        ImGui::End();
    }
    if(statistics_window){
        ImGui::Begin("Statistics", &statistics_window, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Population size: %lu", pop.size());
        ImGui::Text("Generation: %i", pop.generation);
        if(pop.cost_history.size()>0){
            ImGui::Text("Current best: %6.3f", pop.cost_history[pop.cost_history.size()-1]);
            ImVec2 size = ImGui::GetContentRegionAvail();
            static vector<float> bestv(0);
            bestv.push_back(pop.best()->cost);
            ImGui::PlotLines("", bestv.data(), bestv.size(), 0, "", 0, 99/*max cost*/, ImVec2(256, 128));
        }
        ImGui::End();
    }

//        if(ImGui::TabItem("Sync"))
//            camera_sync_test();

    ImGui::End();
}

