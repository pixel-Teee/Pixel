namespace Pixel {
	class MaterialInstance;
	class MaterialInstanceEditor {
	public:
		MaterialInstanceEditor();

		MaterialInstanceEditor(const std::string& virtualPath);

		~MaterialInstanceEditor();

		void OnImGuiRenderer(bool& OpenMaterialInstanceEditor);
	private:
		std::string m_MaterialInstanceVirtualPath;

		Ref<MaterialInstance> m_pMaterialInstance;//material instance
	};
}