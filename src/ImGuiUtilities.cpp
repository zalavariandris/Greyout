//
//  ImGuiUtilities.cpp
//  ofGO_1_0_eyeclass
//
//  Created by András Zalavári on 2017. 12. 08..
//

#include "ImGuiUtilities.hpp"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

void ImGui::Image(shared_ptr<ofImage> image, const ImVec2 &size){
    ImGui::Image((void *)(intptr_t) (image->isAllocated() ? image->getTexture().getTextureData().textureID : 0), size);
}
void ImGui::Image(shared_ptr<ofImage> image){
    ImGui::Image(image, ImVec2(image->getWidth(), image->getHeight()));
}

void ImGui::ClipImage(const char* label, shared_ptr<ofImage> image, const ImVec2 &size, float* x, float* y, float* w, float* h){
    
    Image(image, size);
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;
    
    ImGuiContext& g = *GImGui;
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    //
    
    //        const bool value_changed = DragBehavior(frame_bb, id, v, v_speed, v_min, v_max, decimal_precision, power);
    //
    
    ImVec2 scale = size / ImVec2(image->getWidth(), image->getHeight());
    
    ImRect bb(GetItemRectMin(), GetItemRectMax());
    ImRect clip_bb(bb.Min + ImVec2(*x, *y) * scale,
                   bb.Min + (ImVec2(*x, *y)+ImVec2(*w, *h)) * scale);
    
    window->DrawList->AddRect(clip_bb.Min,
                              clip_bb.Max,
                              GetColorU32(ImGuiCol_Text));
    
    // save current cursor pos
    ImVec2 cursor_pos = ImGui::GetCursorPos();
    ImGui::SetCursorPos(clip_bb.Min - ImGui::GetWindowPos() + ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY()));
    ImGui::Button("##min", ImVec2(20, 20));
    if (ImGui::IsItemActive() && io.MouseDelta.x != 0.0f)
    {
        *x += io.MouseDelta.x/scale.x;
        *y += io.MouseDelta.y/scale.y;
        *w -= io.MouseDelta.x/scale.x;
        *h -= io.MouseDelta.y/scale.y;
        
        
    }
    ImGui::SameLine();
    ImGui::Text("%s [%0.0f,%0.0f]", label, *x, *y);
    ImGui::SetCursorPos(clip_bb.Max - ImGui::GetWindowPos() + ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY())-ImVec2(20, 20));
    
    ImGui::Button("##max", ImVec2(20, 20));
    if (ImGui::IsItemActive() && io.MouseDelta.x != 0.0f)
    {
        *w += io.MouseDelta.x/scale.x;
        *h += io.MouseDelta.y/scale.y;
        
    }
    ImGui::SameLine();
    ImGui::Text("[%0.0f,%0.0f]", *w, *h);
    // restore cursor
    ImGui::SetCursorPos(cursor_pos);
    
    //        window->DrawList->AddText(clip_bb.Min, GetColorU32(ImGuiCol_Text), label);
    
}

void ImGui::Eye(ofEye & eye){
    bool autogain {eye.getAutogain()};
    if(ImGui::Checkbox("autogain", &autogain))
        eye.setAutogain(autogain);
    
    bool autowhitebalance {eye.getAutoWhiteBalance()};
    if(ImGui::Checkbox("autowhitebalance", &autowhitebalance))
        eye.setAutoWhiteBalance(autowhitebalance);
    
    int gain {eye.getGain()};
    if(ImGui::SliderInt("gain", &gain, 0, 63))
        eye.setGain(gain);
    
    int exposure {eye.getExposure()};
    if(ImGui::SliderInt("exposure", &exposure, 0, 255))
        eye.setExposure(exposure);
    
    int sharpness {eye.getSharpness()};
    if(ImGui::SliderInt("sharpness", &sharpness, 0, 255))
        eye.setSharpness(sharpness);
    
    int contrast {eye.getContrast()};
    if(ImGui::SliderInt("contrast", &contrast, 0, 255))
        eye.setContrast(contrast);
    
    int brightness {eye.getBrightness()};
    if(ImGui::SliderInt("brightness", &brightness, 0, 255))
        eye.setBrightness(brightness);
    
    int hue {eye.getHue()};
    if(ImGui::SliderInt("hue", &hue, 0, 255))
        eye.setHue(hue);
    
    int balance[] {eye.getRedBalance(), eye.getGreenBalance(), eye.getBlueBalance()};
    if(ImGui::SliderInt3("balance", balance, 0, 255)){
        eye.setRedBalance(balance[0]);
        eye.setGreenBalance(balance[1]);
        eye.setBlueBalance(balance[2]);
    }
    
    bool flip[] {eye.getFlipH(), eye.getFlipV()};
    if(ImGui::Checkbox("horizontal", &flip[0]))
        eye.setFlip(flip[0], eye.getFlipV());
    ImGui::SameLine();
    if(ImGui::Checkbox("vertical", &flip[1]))
        eye.setFlip(eye.getFlipH(), flip[1]);

//    // Show captured image
//    ImVec2 size(eye.buffer->getWidth(), eye.buffer->getHeight());
//    ImVec2 ava_size = ImGui::GetContentRegionAvail();
//    float scale;
//    if(size.x/size.y > ava_size.x/ava_size.y)
//        scale = ava_size.x/size.x;
//    else
//        scale = ava_size.y/size.y;
//    
//    ImGui::Image((void *)(intptr_t) (eye.buffer->isAllocated() ? eye.buffer->getTexture().getTextureData().textureID : 0), ImVec2(size.x*scale, size.y*scale));
}

void ImGui::App(ofApp & app){
    // Play/Stop/Step
    static bool is_playing {app.play};
    if(ImGui::Button(is_playing ? " Stop " : " Play ")) is_playing = !is_playing;
    ImGui::SameLine();
    app.play = ImGui::Button(">|") or is_playing;
    
    // Vsync
    ImGui::SameLine();
    static bool vsync {true};
    if(ImGui::Checkbox("vsync", &vsync))
        ofSetVerticalSync(vsync);
    
    // Fps
    static vector<float> frame_rates;
    frame_rates.push_back(1/ofGetLastFrameTime());
    if(frame_rates.size()>50)
        frame_rates.erase(frame_rates.begin(), frame_rates.begin()+1);
    
    
    ImGui::SameLine();
    ImGui::PlotLines("", frame_rates.data(), frame_rates.size(), 0, "", 0, 128);
    ImGui::Text("%2.0ffps", ofGetFrameRate());
}

//void ImGui::Pro(Profiler & profiler){
//    std::queue<shared_ptr<const class TreeNode>> q;
//
//    q.push(profiler.root);
//
//    while(!q.empty()) {
//        shared_ptr<const class TreeNode> n = q.front();
//        q.pop();
//        std::cout << " " << n->mLabel;
//
//        for(auto child : n->children){
//            q.push(child);
//        }
//    }
//}

void ImGui::Pop(GA::Population & pop){
    ImGui::Text("Generation: #%i", pop.generation);
    ImGui::Text("Size: %i", (int)pop.size());
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
    ImGui::NewLine();
    
    if(pop.cost_history.size()>0){
        ImGui::Text("Current best: %6.3f", pop.cost_history[pop.cost_history.size()-1]);
        ImVec2 size = ImGui::GetContentRegionAvail();
        ImGui::PlotLines("", pop.cost_history.data(), pop.cost_history.size(), 0, "", 0, 128, size);
    }
}
