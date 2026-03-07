class TrackingModKillCategory: ScriptView
{
	protected ref TrackingModKillCategoryController m_CategoryController;
	protected string m_PreviewClassName;
	protected string m_DisplayName;
	protected int m_KillCount;
	EntityAI m_PreviewObject;
	Widget category_item_preview_container;
	ItemPreviewWidget category_item_preview;
	
	void TrackingModKillCategory(string categoryID, string displayName, int killCount, string previewClassName = "")
	{
		m_PreviewClassName = previewClassName;
		m_DisplayName = displayName;
		m_KillCount = killCount;
		
		if (!m_CategoryController)
			m_CategoryController = TrackingModKillCategoryController.Cast(GetController());
		
		if (m_CategoryController)
		{
			m_CategoryController.CategoryName = m_DisplayName;
			m_CategoryController.NotifyPropertyChanged("CategoryName");
			m_CategoryController.KillCount = m_KillCount.ToString();
			m_CategoryController.NotifyPropertyChanged("KillCount");
		}
	}
	
	void ~TrackingModKillCategory()
	{
		if (m_PreviewObject)
			g_Game.ObjectDelete(m_PreviewObject);
	}
	
	override void OnWidgetScriptInit(Widget w)
	{
		super.OnWidgetScriptInit(w);
		
		if (!m_CategoryController)
			m_CategoryController = TrackingModKillCategoryController.Cast(GetController());
		
		if (!m_CategoryController)
			return;
		
		m_CategoryController.CategoryName = m_DisplayName;
		m_CategoryController.NotifyPropertyChanged("CategoryName");
		m_CategoryController.KillCount = m_KillCount.ToString();
		m_CategoryController.NotifyPropertyChanged("KillCount");
		
		InitializeWidgets();
		LoadCategoryPreview();
	}
	
	void InitializeWidgets()
	{
		if (!GetLayoutRoot())
			return;
		
		category_item_preview_container = GetLayoutRoot().FindAnyWidget("category_item_preview_container");
		category_item_preview = ItemPreviewWidget.Cast(GetLayoutRoot().FindAnyWidget("category_item_preview"));
	}
	
	void LoadCategoryPreview()
	{
		if (!m_CategoryController)
		{
			m_CategoryController = TrackingModKillCategoryController.Cast(GetController());
			if (!m_CategoryController)
			{
				GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(LoadCategoryPreview, 100, false);
				return;
			}
		}
		
		if (m_PreviewClassName == "" || m_PreviewClassName == string.Empty)
		{
			if (category_item_preview_container)
				category_item_preview_container.Show(false);
			return;
		}
		
		if (!category_item_preview_container)
		{
			if (GetLayoutRoot())
				category_item_preview_container = GetLayoutRoot().FindAnyWidget("category_item_preview_container");
			
			if (!category_item_preview_container)
			{
				GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(LoadCategoryPreview, 100, false);
				return;
			}
		}
		
		if (!category_item_preview)
		{
			if (GetLayoutRoot())
				category_item_preview = ItemPreviewWidget.Cast(GetLayoutRoot().FindAnyWidget("category_item_preview"));
			
			if (!category_item_preview)
			{
				GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(LoadCategoryPreview, 100, false);
				return;
			}
		}
		
		if (!m_PreviewObject && g_Game)
		{
			m_PreviewObject = EntityAI.Cast(g_Game.CreateObjectEx(m_PreviewClassName, vector.Zero, ECE_LOCAL|ECE_NOLIFETIME));
			
			if (m_PreviewObject)
			{
				dBodyActive(m_PreviewObject, ActiveState.INACTIVE);
				m_PreviewObject.DisableSimulation(true);
				
				m_CategoryController.Preview = m_PreviewObject;
				m_CategoryController.NotifyPropertyChanged("Preview");
				
				category_item_preview.SetItem(m_PreviewObject);
				category_item_preview.SetModelPosition(Vector(0, 0, 0.5));
				category_item_preview.SetModelOrientation(vector.Zero);
				category_item_preview.SetView(m_PreviewObject.GetViewIndex());
				category_item_preview.Show(true);
			}
		}
		
		if (category_item_preview_container)
		{
			if (m_PreviewObject)
				category_item_preview_container.Show(true);
			else
				category_item_preview_container.Show(false);
		}
	}
	
	override typename GetControllerType()
	{
		return TrackingModKillCategoryController;
	}
	
	override string GetLayoutFile()
	{
		return "Ninjins_LeaderBoard\\GUI\\layouts\\PvE\\trackingModKillCategory_mvc.layout";
	}
}
