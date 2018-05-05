#include "ofApp.hpp"
#include "ofDrawUtilities.hpp"
#include "ImGuiUtilities.hpp"


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
            environment->drawSubsection(0, 0, w, h, camera_clip.x, camera_clip.y, camera_clip.width, camera_clip.height);
        });;
        candidate->cost = compare_images(*candidate->image, *candidate->capture);
    }else if(evaluateMethod == EvaluateCompareGreyscale){
        ofVec2f size = bw_size(candidate->genes->length());
        candidate->capture = make_image(size, [&](int w, int h){
            environment->drawSubsection(0, 0, w, h, camera_clip.x, camera_clip.y, camera_clip.width, camera_clip.height);
        });
        candidate->cost = compare_brightness(*candidate->image, *candidate->capture);
    }else if(evaluateMethod == EvaluateCaptureGreyness){
        ofVec2f size = bw_size(candidate->genes->length());
        candidate->capture = make_image(size, [&](int w, int h){
            environment->drawSubsection(0, 0, w, h, camera_clip.x, camera_clip.y, camera_clip.width, camera_clip.height);
        });
        candidate->cost = compare_brightness_to_grey(*candidate->capture);
    }
}

void ofApp::step_GACamera_coroutine(coroutine<void>::pull_type &VSYNC){
    /* Init cadidates */
    pop.init(70);
    VSYNC();
    
    while(true){
        /* GA loop */
        for(auto i=0; i<pop.size()+cameraLatency; i++){
            /* Pair phenotypes and captures */
            if(i < pop.size()){
                // create phenotype
                decode(pop.candidate(i));
                pop.candidate(i)->image->draw(projection_rect.x, projection_rect.y, projection_rect.width, projection_rect.height);
            }
            if(i >= cameraLatency){
                shared_ptr<GA::Candidate> candidate = pop.candidate(i-cameraLatency);
                shared_ptr<ofImage> capture = camera->buffer;
                evaluate(candidate, capture);
            }
            VSYNC();
        }
        
        // keep a copy of the best candidate
        best_candidate = pop.best();
        best_history.push_back(pop.best()->cost);
        VSYNC();
        
        /* Breed new generation */
        pop.breed();
    }
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
    ImGui::PlotLines ("", fpsv.data(), fpsv.size(), 0, "", 0, 128);ImGui::SameLine();
    ImGui::Text("%0.0f", ofGetFrameRate());
    
    ImGui::DragFloatRange2("projection_rect h",&projection_rect.x, &projection_rect.width);
    ImGui::DragFloatRange2("projection_rect v",&projection_rect.y, &projection_rect.height);
    
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
        ImGui::ClipImage("clip camera",camera->buffer, ImGui::GetContentRegionAvail(), &camera_clip.x, &camera_clip.y, &camera_clip.width, &camera_clip.height);
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
                ImGui::Image((void *)(intptr_t) (candidate->image ? candidate->image->getTexture().getTextureData().textureID : 0), ImVec2(size, size)); ImGui::SameLine();
                if(candidate->image)
                    ImGui::Text("[%0.0fx%0.0f]", candidate->image->getWidth(), candidate->image->getHeight()); ImGui::SameLine();
                
                ImGui::Image((void *)(intptr_t) (candidate->capture ? candidate->capture->getTexture().getTextureData().textureID : 0), ImVec2(size, size)); ImGui::SameLine();
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
            if(ImGui::Button("reset"))
                best_history.clear();
            ImGui::PlotLines("", best_history.data(), best_history.size(), 0, "", 0, 99/*max cost*/, ImVec2(256, 128));
        }
        ImGui::End();
    }

//        if(ImGui::TabItem("Sync"))
//            camera_sync_test();

    ImGui::End();
}

