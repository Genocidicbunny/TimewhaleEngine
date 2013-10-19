
#include "UIManager.h"
#include "WhaleTimer.h"
#include "TWLogger.h"
#include "RenderSystem.h"
#include "ResourceManager.h"
#include "PlatformManager.h"
#include "SceneManager.h"
#include "TextureResource.h"
#include "BinaryFileResource.h"
#include "EventInstancer.h"
#include "EventListener.h"

using namespace std;
using namespace Timewhale;


#pragma region Rocket 
float RocketSystem::GetElapsedTime() {
    return WhaleTimer::getCurrentTime();
}


int RocketSystem::TranslateString(
    Rocket::Core::String& translated, 
    const Rocket::Core::String& input) 
{
    translated = input;
    return 0;
}

bool RocketSystem::LogMessage(
    Rocket::Core::Log::Type type,
    const Rocket::Core::String& message)
{
    switch (type) {
    case Rocket::Core::Log::LT_ERROR:
        log_sxerror("UIManager", message.CString());
        break;
    case Rocket::Core::Log::LT_ASSERT:
        log_sxwarn("UIManager", message.CString());
        break;
    case Rocket::Core::Log::LT_INFO:
        log_sxinfo("UIManager", message.CString());
        break;
    case Rocket::Core::Log::LT_DEBUG:
        log_sxdebug("UIManager", message.CString());
        break;
    }
    return true;
}

// Constructor
RocketRender::RocketRender() {
	
}

RocketRender::~RocketRender() {
}

bool RocketRender::Initialize() {
	auto renderer = RenderSystem::get();
	HRESULT result;

	// create the ui shader
	m_UIShader = new UIShader;
	if(!m_UIShader) {
		return false;
	}

	// initialize the ui shader
	result = m_UIShader->Initialize();
	if(FAILED(result)) {
		log_sxerror("UIManager", "Could not initialize the UI Shader.");
		return false;
	}

	return true;
}
  

void RocketRender::RenderGeometry(
    Rocket::Core::Vertex* vertices, 
    int num_vertices, 
    int* indices, 
    int num_indices, 
    Rocket::Core::TextureHandle texture, 
    const Rocket::Core::Vector2f& translation)
{
    auto renderer = RenderSystem::get();

    // fuck you
}

Rocket::Core::CompiledGeometryHandle RocketRender::CompileGeometry(
    Rocket::Core::Vertex* vertices, 
    int num_vertices, 
    int* indices, 
    int num_indices, 
	Rocket::Core::TextureHandle texture)
{
    auto renderer = RenderSystem::get();
	HRESULT result;

    // Create a new UI object
	UIObject* object = new UIObject;
	if(!object)
	{
		return false;
	}

	// Grab the texinfo for this image
	auto it = renderer->m_TexInfo.find(texture);

	// initialize the ui object
	result = object->Initialize(vertices, num_vertices, indices, num_indices, texture, it->second);
	if(FAILED(result))
	{
		log_sxerror("UIManager", "Could not compile this geometry into a UI object.");
		return false;
	}
 
    // Do nothing
 
    return (Rocket::Core::CompiledGeometryHandle)object;
}

void RocketRender::RenderCompiledGeometry(
    Rocket::Core::CompiledGeometryHandle geometry, 
    const Rocket::Core::Vector2f& translation)
{
	auto renderer = RenderSystem::get();
	if(!renderer) 
		return;
	bool result;
	XMMATRIX orthoMatrix;

	// Get the ortho matrix for 2d rendering
	renderer->mRenderer->GetOrthoMatrix(orthoMatrix);

	// Get the UI object
	UIObject* object = (UIObject*)geometry;

	// Find the texture id associated with the sprite
    auto it = renderer->m_Textures.find(object->GetTextureID());

	// Find the pack associated with the texture id
	auto it2 = renderer->m_Packs.find(it->second);
	
	// Render the UI object with the UI shader
	object->Render();
	result = m_UIShader->Render(object->GetIndexCount(), orthoMatrix,
		renderer->mRenderer->GetScreenWidth(), renderer->mRenderer->GetScreenHeight(), (int)translation.x, (int)translation.y, it2->second->GetTexture());
	
}

void RocketRender::ReleaseCompiledGeometry(
    Rocket::Core::CompiledGeometryHandle geometry)
{
    auto renderer = RenderSystem::get();

	UIObject* object = (UIObject*)geometry;

	object->Shutdown();
	delete object;
}

void RocketRender::EnableScissorRegion(bool enable) {
	enable ? m_UIShader->EnableScissor() : m_UIShader->DisableScissor();
}

void RocketRender::SetScissorRegion(int x, int y, int width, int height) {
    auto renderer = RenderSystem::get();

    D3D11_RECT scissor_rect;
    scissor_rect.left = x;
    scissor_rect.right = x + width;
    scissor_rect.top = y;
    scissor_rect.bottom = y + height;
 
    renderer->mRenderer->GetDeviceContext()->RSSetScissorRects(1, &scissor_rect);
}

bool RocketRender::LoadTexture(
    Rocket::Core::TextureHandle& texture_handle,
    Rocket::Core::Vector2i& texture_dimensions,
    const Rocket::Core::String& source)
{
    //TODO - Add in render/resource calls here.
	Texture tex(source.CString());
	texture_handle = (Rocket::Core::TextureHandle) tex.TextureUID();
	texture_dimensions = Rocket::Core::Vector2i(tex.Width(), tex.Height());
    return true;
}

bool RocketRender::GenerateTexture(
    Rocket::Core::TextureHandle& texture_handle,
    const Rocket::Core::byte* source,
    const Rocket::Core::Vector2i& source_dimensions)
{
    auto renderer = RenderSystem::get();
    auto resources = ResourceManager::get();

    ID3D11Texture2D* texture;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = source_dimensions.x;
	desc.Height = source_dimensions.y;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;

	result = renderer->mRenderer->GetDevice()->CreateTexture2D(&desc, NULL, &texture);
	if(FAILED(result))
	{
		log_sxerror("UIManager", "Could not create a texture2D.");
		return false;
	}
 
	result = renderer->mRenderer->GetDeviceContext()->Map(texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		log_sxerror("UIManager", "Could not map the resource.");
		return false;
	}

	for (int y = 0; y < source_dimensions.y; ++y)
	{
		for (int x = 0; x < source_dimensions.x; ++x)
		{
			const byte* source_pixel = source + (source_dimensions.x * 4 * y) + (x * 4);
			byte* destination_pixel = ((byte*) mappedResource.pData) + mappedResource.RowPitch * y + x * 4;

			//int value;

			destination_pixel[0] = source_pixel[2];
			destination_pixel[1] = source_pixel[1];
			destination_pixel[2] = source_pixel[0];
			destination_pixel[3] = source_pixel[3];
		}
	}
	renderer->mRenderer->GetDeviceContext()->Unmap(texture, 0);

	TextureDetails tex(0, 0, 0, source_dimensions.x, source_dimensions.y);
	TextureDetails details = renderer->CreateTextureDetails(tex, texture, source_dimensions.x, source_dimensions.y);
	texture_handle = (Rocket::Core::TextureHandle) details._tex_id;

    return true;
}

void RocketRender::ReleaseTexture(
    Rocket::Core::TextureHandle texture_handle)
{
    auto renderer = RenderSystem::get();
    auto resources = ResourceManager::get();

    //TODO - Add in render/resource calls here.
}

float RocketRender::GetHorizontalTexelOffset() {
    auto renderer = RenderSystem::get();

    //TODO - Add in render calls here.
    return -0.5f;
}

float RocketRender::GetVerticalTexelOffset() {
    auto renderer = RenderSystem::get();

    //TODO - Add in render calls here.
    return -0.5f;
}

Rocket::Core::FileHandle RocketFile::Open(
    const Rocket::Core::String& path) 
{
    auto resources = ResourceManager::get();

	WhaleFile file(path.CString());

    //TODO - Add in resource calls here.
	return file.getTag();
}

void RocketFile::Close(Rocket::Core::FileHandle file) {
    auto resources = ResourceManager::get();

    //TODO - Add in resource calls here.

	// Unload
}

size_t RocketFile::Read(
    void* buffer, size_t size, Rocket::Core::FileHandle file)
{
    auto resources = ResourceManager::get();

    //TODO - Add in resource calls here.

	WhaleFile rml (file);
	
	
    return rml.Read(buffer, size);
}

bool RocketFile::Seek(Rocket::Core::FileHandle file, long offset, int origin) {
    auto resources = ResourceManager::get();

    //TODO - Add in resource calls here.
	WhaleFile rml (file);

    return rml.Seek(offset, (Timewhale::FILE_POS)origin) == 0;
}

size_t RocketFile::Tell(Rocket::Core::FileHandle file) {
     auto resources = ResourceManager::get();

    //TODO - Add in resource calls here.
	 WhaleFile rml (file);
     return rml.Tell();
}
#pragma endregion

#pragma region UIManager

UIManagerPtr UIManager::sManager = nullptr;

UIManagerPtr const UIManager::create() {
    if(!sManager) {
        sManager = UIManagerPtr(new UIManager());
    }
    return sManager;
}

UIManager::UIManager()
    : mCurrent(nullptr)
{
	actionsToExecute = "";	
}

UIManager::~UIManager() {}

bool UIManager::init() {

    Rocket::Core::SetFileInterface(new RocketFile());
    Rocket::Core::SetSystemInterface(new RocketSystem());

    auto renderInterface = new RocketRender();
    renderInterface->Initialize();

    Rocket::Core::SetRenderInterface(renderInterface);

    bool result = Rocket::Core::Initialise();
	Rocket::Controls::Initialise();
    if (!result) return false;
	EventInstancer* event_instancer = new EventInstancer();
	Rocket::Core::Factory::RegisterEventListenerInstancer(event_instancer);
	event_instancer->RemoveReference();
   
	//Fonts (working!
	bool fontSuccess  = Rocket::Core::FontDatabase::LoadFontFace("NotoSerif-Regular.ttf", "NotoSerif", Rocket::Core::Font::STYLE_NORMAL, Rocket::Core::Font::WEIGHT_NORMAL);
	fontSuccess  = Rocket::Core::FontDatabase::LoadFontFace("ArialBlack.ttf", "ArialBlack", Rocket::Core::Font::STYLE_NORMAL, Rocket::Core::Font::WEIGHT_NORMAL);
	createDebugContext();
	createPauseContext();


    return result;
}
void UIManager::activateCheat(int cheat){
			switch(cheat){
			case 0: 
				actionsToExecute = "give10000resources";
				break;
			case 1:
				actionsToExecute = "remove10000resources";
				break;
			default:
				break;
			}
		
}
void UIManager::update() {
    mHud->Update();
	mPause->Update();

    auto itor = mContexts.begin();
    auto end = mContexts.end();
    
    for(; itor != end; ++itor) {
        itor->second->Update();
    }
}


Rocket::Core::Context* UIManager::current() { return mCurrent; }

Rocket::Core::Context* UIManager::debug() { return mHud; }

Rocket::Core::Context* UIManager::pause() { return mPause; }

void UIManager::createDebugContext() {
    auto platform = PlatformManager::get();
	//Creating new context for HUD
    mHud = Rocket::Core::CreateContext(
        "hud", 
        Rocket::Core::Vector2i(
            platform->width(), platform->height()
            )
        );
	

    //Loading some markup files
    mHudDoc = mHud->LoadDocument("hud");
    if (!mHudDoc) {
        log_sxerror("UIManager", "Failed to load debug ui document.");
		return;
    }
	//Focusing
	mHudDoc->Focus();
	// Remove the caller's reference.
  	mHudDoc->RemoveReference();

	
}
void UIManager::createPauseContext() {
	auto platform = PlatformManager::get();


	//Creating new context for pause
    mPause = Rocket::Core::CreateContext(
        "pause", 
        Rocket::Core::Vector2i(
				platform->width(), platform->height()
            )
        );
	

    //Loading some markup files
    mPauseDoc = mPause->LoadDocument("pause");
    if (!mPauseDoc) {
        log_sxerror("UIManager", "Failed to load pause document.");
		return;
    }
	//Focusing
	mPauseDoc->Focus();
	// Remove the caller's reference.
  	mPauseDoc->RemoveReference();
	pauseVisible = false;
	
}

void UIManager::changeStyleProperty(Rocket::Core::String element, Rocket::Core::String prop, Rocket::Core::String value){

	Rocket::Core::Element* elem = mHudDoc->GetElementById(element);
	if(elem!=NULL){
		elem->SetProperty(prop, value);
	}
}

void UIManager::processEvent(Rocket::Core::Event& event, Rocket::Core::String value)
{	
	//If blank action, set to new action.  
	//NOTE: In implementation, make sure to set actionsToExecute to empty string after processing
	if(actionsToExecute == ""){
		actionsToExecute = value;
	}
}
void UIManager::setButtonFocus(Rocket::Core::String name)
{
	Rocket::Core::Element* elem = mHudDoc->GetElementById(name);
	if(elem != NULL) elem->Focus();
}
void UIManager::toggleDebugContext() {
    debugVisible = !debugVisible;
    if (debugVisible) {
         mHudDoc->Show();
    }
    else {
        mHudDoc->Hide();
    }

}
void UIManager::setTextById(Rocket::Core::String id, Rocket::Core::String text){
	Rocket::Core::Element* element = mHudDoc->GetElementById(id);
	element->SetInnerRML(text);
}
void UIManager::tryPause(){
	actionsToExecute = "tryPause";
}
bool UIManager::isMouseOverUI(){
	if(!mHudDoc) return false;
	if(!mHudDoc->IsVisible() ) return false;
	Rocket::Core::String id = mHud->GetHoverElement()->GetId();
	if(id=="content"||id=="tutorial"||id=="cinematic"||id=="cinematictext"||
		id=="cinematicscientisttext"||id=="tutorialtext"||id=="menusidebar"||id=="menucontainer"||
		id=="cinematicclicktext" || id=="cinematicscientist") return false;
	return true;
}
bool UIManager::isHudVisible(){
	return mHudDoc->IsVisible();
}
Rocket::Core::String* UIManager::getAction(){
	return &actionsToExecute;   
}
void UIManager::togglePauseContext(){
	pauseVisible = !pauseVisible;
	if(pauseVisible){
		actionsToExecute = "showPause";
		mPauseDoc->Show();
	}
	else{
		actionsToExecute = "hidePause";
		mPauseDoc->Hide();
	}
	
}
void UIManager::confirmActionReceived(){ 
	actionsToExecute = "";
}
void UIManager::showContext(){
	if(mHudDoc!=NULL)
	mHudDoc->Show();
}
Rocket::Core::Context* UIManager::getContext(){
	if(pauseVisible) return mPause;
	else return mHud;
}

void UIManager::hideContext(){
	if(mHudDoc!=NULL){
		mHudDoc->Hide();	
	}
}

void UIManager::showPause(){
	if(mHudDoc!=NULL){
		mHudDoc->Show();
		pauseVisible = true;
	}
}

void UIManager::hidePause(){
	if(mPauseDoc!=NULL){
		mPauseDoc->Hide();
		pauseVisible = false;
	}
}

void UIManager::render() {
    mHud->Render();
	mPause->Render();
}

void UIManager::shutdown() {

}

void UIManager::uiKey(char key, bool up, uint32_t keyMods) {
	auto scene = SceneManager::GetCurrentScene();
   // if (!scene) return;

   // if (mContexts.count(scene->getSceneID()) < 1) return;
    
		Rocket::Core::Context* context;
	if(pauseVisible){
		context = mPause;
	}
	else {
		context = mHud;
	}
    
    char transKey = key;
    if (key >= 'a' && key <= 'z')
        transKey -= 'a' + Rocket::Core::Input::KeyIdentifier::KI_A;
    else if (key >= 'A' && key <= 'Z')
        transKey -= 'A' + Rocket::Core::Input::KeyIdentifier::KI_A;
    else if (key >= '0' && key <= '9')
        transKey -= '0' + Rocket::Core::Input::KeyIdentifier::KI_1;
    
    if (up)
        context->ProcessKeyUp((Rocket::Core::Input::KeyIdentifier)transKey, keyMods);
    else
        context->ProcessKeyDown((Rocket::Core::Input::KeyIdentifier)transKey, keyMods);
}

void UIManager::uiText(char key, uint32_t keyMods) {
	
    
	Rocket::Core::Context* context;
	if(pauseVisible){
		context = mPause;
	}
	else {
		context = mHud;
	}
    context->ProcessTextInput(Rocket::Core::word(key));
}

void UIManager::uiText(const std::string &str, uint32_t keyMods) {

    
    Rocket::Core::Context* context;
	if(pauseVisible){
		context = mPause;
	}
	else {
		context = mHud;
	}
    context->ProcessTextInput(Rocket::Core::String(str.c_str()));
}

void UIManager::uiMouseMove(int x, int y, uint32_t keyMods) {
	///auto scene = SceneManager::GetCurrentScene();
   /// if (!scene) return;

   /// if (mContexts.count(scene->getSceneID()) < 1) return;
    
	Rocket::Core::Context* context;
	if(pauseVisible){
		context = mPause;
	}
	else {
		context = mHud;
	}
    context->ProcessMouseMove(x, y, keyMods);
}

void UIManager::uiMouseBtn(int button, bool up, uint32_t keyMods) {
   // auto scene = SceneManager::GetCurrentScene();
    //if (!scene) return;

   // if (mContexts.count(scene->getSceneID()) < 1) return;
	Rocket::Core::Context* context;
	if(pauseVisible){
		context = mPause;
	}
	else {
		context = mHud;
	}
    if (up)
        context->ProcessMouseButtonUp(button, keyMods);
    else
        context->ProcessMouseButtonDown(button, keyMods);
}

void UIManager::uiMouseWheel(int delta, uint32_t keyMods) {
    auto scene = SceneManager::GetCurrentScene();
    if (!scene) return;

    if (mContexts.count(scene->getSceneID()) < 1) return;
    
    auto context = mContexts.at(scene->getSceneID());
    context->ProcessMouseWheel(delta, keyMods);
}
#pragma endregion