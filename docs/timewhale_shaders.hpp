
//Constant Buffer Impl
struct ConstantBuffer {
	ID3D11Buffer *constantBuf;
	ubyte *bufferData;
	size_t bufferSize;
	bool dirty;

	ConstantBuffer()
	   :constantBuf(nullptr),
		bufferData(nullptr),
		bufferSize(0),
		dirty(false)
	{};
	~ConstantBuffer() {
		SAFE_RELEASE(constantBuf);
		if(bufferData) delete bufferData;
	}
};

struct Constant {
	std::string name;
	ubyte *vsData;
	ubyte *psData;
	ubyte *gsData;
	ConstantBuffer* vsConstBuf;
	ConstantBuffer* psConstBuf;
	ConstantBuffer* gsConstBuf;

	Constant(const std::string& _n)
		:name(_n),
		vsData(nullptr),
		psData(nullptr),
		gsData(nullptr),
		vsConstBuf(nullptr),
		psConstBuf(nullptr),
		gsConstBuf(nullptr)
	{};
	//We dont care about destruction since all of Constant's pointers are non-owning
};

struct Sampler {
	std::string name;
	int vsBind;
	int psBind;
	int gsBind;

	Sampler(const std::string& _n)
		:name(_n),
		vsBind(-1),
		psBind(-1),
		gsBind(-1)
	{}
};

#define SAFE_RELEASE(p) {if(p) p->Release(); p = nullptr;}

struct Shader
{
	friend class Material
	InstanceID shaderID;
	std::string shaderName;
	//These stay the same for us
	ID3D11VertexShader *vertexShader;
	ID3D11PixelShader *pixelShader;
	ID3D11GeometryShader *geometryShader;
	ID3DBlob *inputSignature;

	//Rework the other parts as slightly more heavyweight but easier to reason
	std::vector<ConstantBuffer> vsConstants;
	std::vector<ConstantBuffer> psConstants;
	std::vector<ConstantBuffer> gsConstants;

	std::unordered_map<std::string, Constant> constants;

	std::unordered_map<std::string, Sampler> textures;
	std::unordered_map<std::string, Sampler> samplers;

	//however we store these. Might want to do this outside the shader and directly in the renderer
	//16 will work for any hardware right now, 16 is the max samplers per shader
	DXSamplerState* vsSamplerStates[16];
	DXSamplerState* psSamplerStates[16];
	DXSamplerState* gsSamplerStates[16];

	DXTexture* vsTextures[16];
	DXTexture* psTextures[16];
	DXTexture* gsTextures[16];

	Shader(const std::string _n)
		:shaderID(GetNextReservedID()),
		shaderName(_n),
		vertexShader(nullptr),
		pixelShader(nullptr),
		geometryShader(nullptr),
		inputSignature(nullptr),
		vsConstants(),
		psConstants(),
		gsConstants(),
		constants(),
		textures(),
		samplers()
	{}
	~Shader() {
		SAFE_RELEASE(vertexShader);
		SAFE_RELEASE(pixelShader);
		SAFE_RELEASE(geometryShader);
		SAFE_RELEASE(inputSignature);
		//The rest will get cleaned out properly on their own since we're storing the objects
		//and not pointers
		//We dont clear out the sampler and texture arrays since those only store non-owning
		//pointers, thus we're not responsible for lifetime
	}
	void setSamplerState(const std::string& name, DXSamplerState* sampler) {
		trySetSamplerState(name, sampler, false);
	}
	void setTexture(const std::string& name, DXTexture* texture) {
		trySetTexture(name, texture, false);
	}
	void resetSamplerStates() {
		ZeroMemory(vsSamplerStates, 16 * sizeof(DXSamplerState*));
		ZeroMemory(psSamplerStates, 16 * sizeof(DXSamplerState*));
		ZeroMemory(gsSamplerStates, 16 * sizeof(DXSamplerState*));
	}
	void resetTextures() {
		ZeroMemory(vsTextures, 16 * sizeof(DXTexture*));
		ZeroMemory(psTextures, 16 * sizeof(DXTexture*));
		ZeroMemory(gsTextures, 16 * sizeof(DXTexture*));
	}

	void reset() {
		resetSamplerStates();
		resetTextures();
	}

	void setConstanti(const std::string& name, const int constant, const size_t offset = 0) {
		setRawConstant(name, &constant, sizeof(constant), _offset, false);
	}
	void setConstantf(const std::string& name, const float constant, const size_t _offset = 0) {
		setRawConstant(name, &constant, sizeof(constant), _offset, false);
	}
	void setConstantNi(const std::string& name, const int* constant, const size_t count, const size_t _offset = 0) {
		setRawConstant(name, constant, count * sizeof(int), _offset, false);
	}
	void setConstantNf(const std::string& name, const int* constant, const size_t count, const size_t _offset = 0) {
		setRawConstant(name, constant, count * sizeof(float), _offset, false);
	}

protected:
	void trySetTexture(const std::string& name, DXTexture* texture, bool quiet = true) {
		auto finder = textures.find(name);
		if(finder == textures.end()) {
			if(!quiet)
				log_sxerror("Shader", "Error, no texture by name %s exists in shader %s to set", name.c_str(), shaderName.c_str());
			return;
		}
		auto& tex = finder->second;
		if(tex.vsIndex >= 0) {
			vsTextures[tex.vsIndex] = texture;			
		}
		if(tex.psIndex >= 0) {
			psTextures[tex.psIndex] = texture;
		}
		if(tex.gsIndex >= 0) {
			gsTextures[tex.gsIndex] = texture;
		}
	}
	void trySetSamplerState(const std::string& name, DXTexture* texture, bool quiet = true) {
		auto finder = samplers.find(name);
		if(finder == samplers.end()) {
			if(!quiet)
				log_sxerror("Shader", "Error, no sampler by name %s exists in shader %s to set", name.c_str(), shaderName.c_str());
			return;
		}
		auto& samp = finder->second;
		if(samp.vsIndex >= 0) {
			vsSamplerStates[samp.vsIndex] = sampler;
		}
		if(samp.psIndex >= 0) {
			psSamplerStates[samp.psIndex] = sampler;
		}
		if(samp.gsIndex >= 0) {
			gsSamplerStates[samp.gsIndex] = sampler;
		}
	}
	void setRawConstant(
		const std::string& name,
		const void* data, 
		const size_t size,
		const size_t offset = 0,
		const bool quiet = true)
	{
		if(!data) return;
		auto finder = constants.find(name);
		if(finder == constants.end()) {
			if(!quiet)
				log_sxerror("Shader", "Error, no constants by name %s exists in shader %s to set", name.c_str(), shadername.c_str());
			return;
		}
		auto& constant = finder->second;
		auto offset_data = data + offset;
		if(constant.vsData) {
			if(memcmp(constants.vsData, offset_data, size)) {
				memcpy(constants.vsData, offset_data, size);
				assert(constants.vsConstBuf);
				constants.vsConstBuf->dirty = true;
			}
		}
		if(constants.psData) {
			if(memcmp(constants.psData, offset_data, size)) {
				memcpy(constants.psData, offset_data, size);
				assert(constants.psConstBuf);
				constants.psConstBuf->dirty = true;
			}
		}
		if(constants.gsData) {
			if(memcmp(constants.gsData, offset_data, size)) {
				memcpy(constants.gsData, offset_data, size);
				assert(constants.gsConstBuf);
				constants.gsConstBuf->dirty = true;
			}
		}
	}
	void applyConstants(
		ID3D11DeviceContext* context) 
	{
		//Applies all currently set constants to the passed in context
		//VS constants
		for(auto& cbuf : vsConstants) {
			//Only update if data is dirty. Save some bw
			if(cbuf.dirty) {
				context->UpdateSubresource(cbuf.constantBuf, 0, NULL, cbuf.bufferData, 0, 0);
				cbuf.dirty = false;
			}
		}
		for(auto& cbuf : psConstants) {
			if(cbuf.dirty) {
				context->UpdateSubresource(cbuf.constantBuf, 0, NULL, cbuf.bufferData, 0, 0);
				cbuf.dirty = false;
			}
		}
		for(auto& cbuf : gsConstants) {
			if(cbuf.dirty) {
				context->UpdateSubresource(cbuf.constantBuf, 0, NULL, cbuf.bufferData, 0, 0);
				cbuf.dirty = false;
			}
		}
	}
	void applySamplerStates(
		ID3D11DeviceContext* context) 
	{
		//can modify this to find the min/max samplers and only set the ones needed.
		//Since any gaps in the middle would still be null though, this call should be safe
		//even if all we've got is one sampler
		context->VSSetSamplers(0, 16, vsSamplerStates);
		context->PSSetSamplers(0, 16, psSamplerStates);
		context->GSSetSamplers(0, 16, gsSamplerStates);
	}
	void applyTextures(
		ID3D11DeviceContext* context) 
	{
		ID3D11ShaderResourceView* srvArray[16];
		ZeroMemory(srvArray, 16 * sizeof(ID3D11ShaderResourceView*));
		for(size_t i = 0; i < 16; ++i) {
			auto tex = vsTextures[i];
			if(tex && tex->GetTexture()) {
				srvArray[i] = tex->GetTexture();
			} 

		}
		context->VSSetShaderResources(0, 16, srvArray);
		ZeroMemory(srvArray, 16 * sizeof(ID3D11ShaderResourceView*));
		for(size_t i = 0; i < 16; ++i) {
			auto tex = psTexture[i];
			if(tex && tex->GetTexture()) {
				srvArray[i] = tex->GetTexture();
			}
		}
		context->PSSetShaderResources(0, 16, srvArray);
		ZeroMemory(srvArray, 16 * sizeof(ID3D11ShaderResourceView*));
		for(size_t i = 0; i < 16; ++i) {
			auto tex = gsTexture[i];
			if(tex && tex->GetTexture()) {
				srvArray[i] = tex->GetTexture();
			}
		}
		context->GSSetShaderResources(0, 16, srvArray);
	}
	void set(
		ID3D11DeviceContext* context) 
	{
		if(vertexShader) {
			context->VSSetShader(vertexShader, NULL, 0);
			for(size_t i = 0; i < vsConstants.size(); ++i) {
				context->VSSetConstantBuffers(i, 1, vsConstants.at(i).constantBuf);
			}
		}
		if(pixelShader) {
			context->PSSetShader(pixelShader, NULL, 0);
			for(size_t i = 0; i < psConstants.size(); ++i) {
				context->PSSetConstantBuffers(i, 1, psConstants.at(i).constantBuf);
			}	
		}
		if(geometryShader) {
			context->GSSetShader(geometryShader, NULL, 0);
			for(size_t i = 0; i < gsConstants.size(); ++i) {
				context->GSSetConstantBuffers(i, 1, gsConstants.at(i).constantBuf);
			}
		}
	}

};

struct Pass {
	Shader* 
};

struct Material 
{
	InstanceID materialID;
	std::vector<Pass>

};


//Since we're precompiling the shaders, we dont need all the extra parameters
//Assume this is a function within the renderer thus we have access to private render members
Shader* CreateShader(
	const std::string& name,
	const std::string& vsFile,
	const std::string& psFile,
	const std::string& gsFile = "")
{
	Shader newShader = new Shader(name);
	ID3D11ShaderReflection *vsReflect(nullptr);
	ID3D11ShaderReflection *psReflect(nullptr);
	ID3D11ShaderReflection *gsReflect(nullptr);

	if(!vsFile.empty()) {
		auto vsFileRes = new WhaleFile(vsFile);
		if(!vsFileRes) {
			log_sxerror("Shader", "Error opening VS file %s. Cannot continue", vsFile.c_str());
			delete newShader;
			return nullptr;
		}
		auto vsFilePtr = vsFileRes->GetMem();
		HRESULT result = 
			device->CreateVertexShader(vsFilePtr, vsFileRes.Size(), NULL, &newShader->vertexShader);
		if(FAILED(result)) {
			log_sxerror("Shader", "Could not create Vertex shader from %s. Cannot continue!", vsFile.c_str());
			//Clean up after ourselves before returning
			delete vsFileRes;
			delete newShader;
			return nullptr;
		}
		//Grab the input signature from the vertex shader
		result = D3DGetInputSignatureBlob(vsFilePtr, vsFileRes.Size(), &newShader->inputSignature);	
		if(FAILED(result)) {
			log_sxerror("Shader", "Could not get Input signature from %s. Cannot continue!", vsFile.c_str());
			delete vsFileRes;
			delete newShader;
			return nullptr;
		}
		result = D3DReflect(vsFilePtr, vsFileRes.Size(), IID_ID3D11ShaderReflection, (void **)&vsReflect);
		if(FAILED(result)) {
			log_sxerror("Shader", "Could not get Reflection Interface from %s. Cannot continue!", vsFile.c_str());
			delete vsFileRes;
			delete newShader;
			return nullptr;
		}
		delete vsFileRes;
	}
	if(!psFile.empty()) {
		auto psFileRes = new WhaleFile(psFile);
		if(!psFileRes) {
			log_sxerror("Shader", "Error opening PS file %s. Cannot continue!", psFile.c_str());
			delete newShader;
			SAFE_RELEASE(vsReflect);
			return nullptr;
		}
		auto psFilePtr = psFileRes->GetMem();
		HRESULT result =
			device->CreatePixelShader(psFilePtr, psFileRes.Size(), NULL, &newShader->pixelShader);
		if(FAILED(result)) {
			log_sxerror("Shader", "Could not create Pixel shader from %s. Cannot continue!", psFile.c_str());
			delete psFileRes;
			SAFE_RELEASE(vsReflect);
			delete newShader;
			return nullptr;
		}
		result = D3DReflect(psFilePtr, psFileRes.Size(), IID_ID3D11ShaderReflection, (void **)&psReflect);
		if(FAILED(result)) {
			log_sxerror("Shader", "Could not get Reflection Interface from %s. Cannot continue!", psFile.c_str());
			delete psFileRes;
			SAFE_RELEASE(vsReflect);
			delete newShader;
			return nullptr;
		}
		delete psFileRes;
	}
	if(!gsFile.empty()) {
		auto gsFileRes = new WhaleFile(gsFile);
		if(!gsFileRes) {
			log_sxerror("Shader", "Error opening PS file %s. Cannot continue!", gsFile.c_str());
			delete newShader;
			SAFE_RELEASE(vsReflect);
			return nullptr;
		}
		auto gsFilePtr = gsFileRes->GetMem();
		HRESULT result =
			device->CreatePixelShader(gsFilePtr, gsFileRes.Size(), NULL, &newShader->geometryShader);
		if(FAILED(result)) {
			log_sxerror("Shader", "Could not create Pixel shader from %s. Cannot continue!", gsFile.c_str());
			delete gsFileRes;
			SAFE_RELEASE(vsReflect);
			SAFE_RELEASE(psReflect);
			delete newShader;
			return nullptr;
		}
		result = D3DReflect(gsFilePtr, gsFileRes.Size(), IID_ID3D11ShaderReflection, (void **)&gsReflect);
		if(FAILED(result)) {
			log_sxerror("Shader", "Could not get Reflection Interface from %s. Cannot continue!", gsFile.c_str());
			delete gsFileRes;
			SAFE_RELEASE(psReflect);
			SAFE_RELEASE(vsReflect);
			delete newShader;
			return nullptr;
		}
		delete gsFileRes;
	}
	bool res = GenerateShaderConstants(newShader, vsReflect, psReflect, gsReflect);
	if(!res) {
		log_sxerror("Shader", "Error Generating shader constants for shader %s.", name.c_str());
		SAFE_RELEASE(vsReflect);
		SAFE_RELEASE(psReflect);
		SAFE_RELEASE(gsReflect);
		delete newShader;
		return nullptr;
	}
	res = GenerateShaderSamplers(newShader, vsReflect, psReflect, gsReflect);
	if(!res) {
		log_sxerror("Shader", "Error Generating shader samplers for shader %s.", name.c_str());
		SAFE_RELEASE(vsReflect);
		SAFE_RELEASE(psReflect);
		SAFE_RELEASE(gsReflect);
		delete newShader;
		return nullptr;
	}
	res = ValidateShader(newShader, vsReflect, psReflect, gsReflect);
	if(!res) {
		log_sxerror("Shader", "Error validating shader %s", name.c_str());
		delete newShader;
		newShader = nullptr;	
	}
	SAFE_RELEASE(vsReflect);
	SAFE_RELEASE(psReflect);
	SAFE_RELEASE(gsReflect);
	return newShader;
}

bool GenereateShaderConstants(
	Shader* shader, 
	ID3D11ShaderReflection* vsR, 
	ID3D11ShaderReflection* psR, 
	ID3D11ShaderReflection* gsR) 
{
	if(!shader) return false;
	HRESULT result;
	D3D11_SHADER_DESC vsD, psD, gsD;
	ZeroMemory(&vsD, sizeof(vsD));
	ZeroMemory(&psD, sizeof(psD));
	ZeroMemory(&gsD, sizeof(gsD));
	if(vsR) {
		vsR->GetDesc(&vsD);
		if(vsD.ConstantBuffers) {
			//We construct the ConstantBuffer objects here as one big chunk. We'll still
			//have to populate them, but we won't be pushing them one by one. 
			//In addition, this guarantees that at(buffer_number) will give us the right cbuf
			shader->vsConstants.resize(vsD.ConstantBuffers);
		}
	}
	if(psR) {
		psR->GetDesc(&psD);
		if(psD.ConstantBuffers) {
			shader->psConstants.resize(psD.ConstantBuffers);
		}
	}
	if(gsR) {
		gsR->GetDesc(&gsD);
		if(gsD.ConstantBuffers) {
			shader->gsConstants.resize(gsD.ConstantBuffers);
		}
	}

	D3D11_SHADER_BUFFER_DESC sbDesc;

	D3D11_BUFFER_DESC constDesc;
	constDesc.Usage = D3D11_USAGE_DEFAULT;
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.CPUAccessFlags = 0;
	constDesc.MiscFlags = 0;

	for(size_t i = 0; i < vsD.ConstantBuffers; ++i) {
		auto& Cbuf = shader->vsConstants.at(i);
		auto IndexedCBuf = vsR->GetConstantBufferByIndex(i);
		IndexedCBuf->GetDesc(&sbDesc);

		constDesc.ByteWidth = sbDesc.Size;
		result = device->CreateBuffer(&constDesc, NULL, &CBuf.constantBuf);
		if(FAILED(result)) {
			log_sxerror("Shader", "Error creating a buffer for VS.CBuffer %s", sbDesc.Name);
			return false;
		}
		CBuf.bufferData = new ubyte[sbDesc.Size];
		CBuf.bufferSize = sbDesc.Size;
		for(size_t c = 0; c < sbDesc.Variables; ++c) {
			D3D11_SHADER_VARIABLE_DESC varDesc;
			IndexedCBuf->GetVariableByIndex(c)->GetDesc(&varDesc);

			auto placer = 
				shader->constants.emplace(make_pair(varDesc.Name, Constant(varDesc.Name)));
			auto& constant = placer.first->second;

			constant.vsData = Cbuf.bufferData + varDesc.StartOffset;
			constant.vsBuffer = &CBuf;
		}
		Cbuf.dirty = false;
	}
	for(size_t i = 0; i < psD.ConstantBuffers; ++i) {
		auto& CBuf = shader->psConstants.at(i);
		auto IndexedCBuf = psR->GetConstantBufferByIndex(i);
		IndexedCBuf->GetDesc(sbDesc);

		constDesc.ByteWidth = sbDesc.Size;
		result = device->CreateBuffer(&constDesc, NULL, &Cbuf.constantBuf);
		if(FAILED(result)) {
			log_sxerror("Shader", "Error creating a buffer for PS.CBuffer %s", sbDesc.Name);
			return false;
		}
		CBuf.bufferData = new ubyte[sbDesc.Size];
		CBuf.bufferSize = sbDesc.Size;
		for(size_t c = 0; c < sbDesc.Variables; ++c) {
			D3D11_SHADER_VARIABLE_DESC varDesc;
			IndexedCBuf->GetVariableByIndex(c)->GetDesc(&varDesc);
			auto finder =
				shader->constants.find(varDesc.Name);
			if(finder == shader->constants.end()) {
				finder = shader->constants.emplace(make_pair(varDesc.Name, Constant(varDesc.name))).first;
			}
			//or maybe finder.second
			auto& constant = finder->second;
			constant.psData = CBuf.bufferData + varDesc.StartOffset;
			constant.psBuffer = &CBuf;
		}
		CBuf.dirty = false;
	}
	for(size_t i = 0; i < gsD.ConstantBuffers; ++i) {
		auto& CBuf = shader->gsConstants.at(i);
		auto IndexedCBuf = gsR->GetConstantBufferByIndex(i);
		IndexedCBuf->GetDesc(sbDesc);

		constDesc.ByteWidth = sbDesc.Size;
		result = device->CreateBuffer(&constDesc, NULL, &Cbuf.constantBuf);
		if(FAILED(result)) {
			log_sxerror("Shader", "Error creating a buffer for PS.CBuffer %s", sbDesc.Name);
			return false;
		}
		CBuf.bufferData = new ubyte[sbDesc.Size];
		CBuf.bufferSize = sbDesc.Size;
		for(size_t c = 0; c < sbDesc.Variables; ++c) {
			D3D11_SHADER_VARIABLE_DESC varDesc;
			IndexedCBuf->GetVariableByIndex(c)->GetDesc(&varDesc);
			auto finder =
				shader->constants.find(varDesc.Name);
			if(finder == shader->constants.end()) {
				finder = shader->constants.emplace(make_pair(varDesc.Name, Constant(varDesc.name))).first;
			}
			//or maybe finder.second
			auto& constant = finder->second;
			constant.gsData = CBuf.bufferData + varDesc.StartOffset;
			constant.gsBuffer = &CBuf;
		}
		CBuf.dirty = false;
	}
	return true;
}

bool GenerateShaderSamplers(
	Shader* shader, 
	ID3D11ShaderReflection* vsR, 
	ID3D11ShaderReflection* psR, 
	ID3D11ShaderReflection* gsR) 
{
	if(!shader) return false;
	size_t vsRes, psRes, gsRes;
	vsRes = psRes = gsRes = 0;
	HRESULT result;
	D3D11_SHADER_DESC vsD, psD, gsD;
	ZeroMemory(&vsD, sizeof(vsD));
	ZeroMemory(&psD, sizeof(psD));
	ZeroMemory(&gsD, sizeof(gsD));
	if(vsR) {
		vsR->GetDesc(&vsD);
		vsRes = vsR.BoundResources;
	}
	if(psR) {
		psR->GetDesc(&psD);
		psRes = psR.BoundResources;
	}
	if(gsR) {
		gsR->GetDesc(&gsD);
		gsRes = gsR.BoundResources;
	}
	D3D11_SHADER_INPUT_BIND_DESC sibDesc;
	for(size_t i = 0; i < vsRes; ++i) {
		result = vsR->GetResourceBindingDesc(i, &sibDesc);
		if(FAILED(result)) {
			log_sxerror("Shader", "Error getting resource binding description");
			return false;
		}
		if(sibDesc.Type == D3D11_SIT_TEXTURE) {
			auto placer =
				shader->textures.emplace(make_pair(sibDesc.Name, Sampler(sibDesc.Name)));
			auto& tex = placer.first->second;
			tex.vsIndex = sibDesc.BindPoint;
		} else if(sibDesc.Type == D3D11_SIT_SAMPLER) {
			auto placer =
				shader->samplers.emplace(make_pair(sibDesc.Name, Sampler(sibDesc.Name)));
			auto& samp = placer.first->second;
			samp.vsIndex = sibDesc.BindPoint;
		}
	}
	for(size_t i = 0; i < psRes; ++i) {
		result = psR->GetResourceBindingDesc(i, &sibDesc);
		if(FAILED(result)) {
			log_sxerror("Shader", "Error getting resource binding description");
			return false;
		}
		if(sibDesc.Type == D3D11_SIT_TEXTURE) {
			auto finder =
				shader->textures.find(sibDesc.Name);
			if(finder == shader->textures.end()) {
				finder = shader->textures.emplace(make_pair(sibDesc.Name, Sampler(sibDesc.Name))).first;
			}
			auto& tex = finder->second;
			tex.psIndex = sibDesc.BindPoint;
		}
		else if (sibDesc.Type == D3D11_SIT_SAMPLER) {
			auto finder = 
				shader->samplers.find(sibDesc.Name);
			if(finder == shader->samplers.end()) {
				finder = shader->samplers.emplace(make_pair(sibDesc.name, Sampler(sibDesc.Name))).first;
			}
			auto& samp = finder->second;
			samp.psIndex = sibDesc.BindPoint;
		}
	}
	for(size_t i = 0; i < gsRes; ++i) {
		result = gsR->GetResourceBindingDesc(i, &sibDesc);
		if(FAILED(result)) {
			log_sxerror("Shader", "Error getting resource binding description");
			return false;
		}
		if(sibDesc.Type == D3D11_SIT_TEXTURE) {
			auto finder =
				shader->textures.find(sibDesc.Name);
			if(finder == shader->textures.end()) {
				finder = shader->textures.emplace(make_pair(sibDesc.Name, Sampler(sibDesc.Name))).first;
			}
			auto& tex = finder->second;
			tex.gsIndex = sibDesc.BindPoint;
		}
		else if (sibDesc.Type == D3D11_SIT_SAMPLER) {
			auto finder = 
				shader->samplers.find(sibDesc.Name);
			if(finder == shader->samplers.end()) {
				finder = shader->samplers.emplace(make_pair(sibDesc.name, Sampler(sibDesc.Name))).first;
			}
			auto& samp = finder->second;
			samp.gsIndex = sibDesc.BindPoint;
		}
	}
	return true;
}

bool ValidateShader(
	Shader* shader,
	ID3D11ShaderReflection* vsR,
	ID3D11ShaderReflection* psR,
	ID3D11ShaderReflection* gsR)
{
	if(!shader) return false; 	//need a shader
	if(!vsR) return false;		//and at least a vertex shader
	if(!psR && !gsR) return true; //Only vertex shader, fine for us for now

	HRESULT result;
	D3D11_SHADER_DESC vsD, gsD, psD;
	ZeroMemory(&vsD, sizeof(vsD));
	ZeroMemory(&psD, sizeof(psD));
	ZeroMemory(&gsD, sizeof(gsD));

	vsR->GetDesc(&vsD);
	if(psR) psR->GetDesc(&psD);
	if(gsR) gsR->GetDesc(&gsD);

	//Since the geometry shader has a funky signature thing, im only validating vsD -> psD
	if(psD->InputParameters > vsD->OutputParameters) return false; //Input needs more parameters than output provides, automatic failure
	D3D11_SIGNATURE_PARAMETER_DESC inDesc, outDesc;
	for(size_t i = 0; i < psD->InputParameters) {
		psR->GetInputParameterDesc(i, &inDesc);
		vsR->GetOutputParameterDesc(i, &outDesc);
		if(!strcmp(inDesc.SemanticName, outDesc.SemanticName) ||
			inDesc.SemanticIndex != outDesc.SemanticIndex ||
			inDesc.Register != outDesc.Register ||
			inDesc.SystemValueType != outDesc.SystemValueType ||
			inDesc.ComponentType != outDesc.ComponentType ||
			inDesc.Mask != outDesc.Mask) {
			log_sxerror("Shader", "Shader Signature Mismatch. VS.Output %s%d does not match PS.Input %s%d",
				inDesc.SemanticName, inDesc.SemanticIndex, outDesc.SemanticName, outDesc.SemanticIndex);
			return false;
		}
	}
	return true;
}


