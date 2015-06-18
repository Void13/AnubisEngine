#ifndef H__ERRORLIST
#define H__ERRORLIST

ADD_ERROR(FileNotFound);

ADD_ERROR(CreateWindowFailed);
ADD_ERROR(DeviceCreationFailed);
ADD_ERROR(SwapBuffersFailed);
ADD_ERROR(ResetDeviceFailed);

ADD_ERROR(EventCreatingFailed);
ADD_ERROR(EventSystemNotInitialized);
ADD_ERROR(EventCallingFailed);
ADD_ERROR(EventKillingFailed);

ADD_ERROR(InputDeviceInitFailed);
ADD_ERROR(ObjectModelCreatingFailed);

ADD_ERROR(CreatingContantBufferFailed);
ADD_ERROR(CreatingSamplerStateFailed);
ADD_ERROR(CreatingBlenderStateFailed);
ADD_ERROR(CreatingDepthStencilStateFailed);
ADD_ERROR(CreatingRastStateFailed);

ADD_ERROR(UnableToFindShader);
ADD_ERROR(UnableToCompileShader);
ADD_ERROR(SOCreatingFailed);
ADD_ERROR(VertexShaderCreatingFailed);
ADD_ERROR(PixelShaderCreatingFailed);
ADD_ERROR(GeometryShaderCreatingFailed);
ADD_ERROR(HullShaderCreatingFailed);
ADD_ERROR(DomainShaderCreatingFailed);
ADD_ERROR(ComputeShaderCreatingFailed);
ADD_ERROR(InputLayoutCreationFailed);

ADD_ERROR(ScreenshotTakeFailed);

ADD_ERROR(PhysXError);
ADD_ERROR(SimplifyError);
ADD_ERROR(DevILError);

ADD_ERROR(D3D11ResourceFailed);

ADD_ERROR(Model_CreatingInstanceBufferFailed);
ADD_ERROR(Model_RenderFailed);
ADD_ERROR(ResourceMapFailed);

ADD_ERROR(ModelLoadingError);

ADD_ERROR(ResourceManager);
ADD_ERROR(FileReading);

#endif