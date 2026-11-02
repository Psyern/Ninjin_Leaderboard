class TrackingModShopMenu: ScriptViewMenu
{
	protected static ref TrackingModShopMenu s_NJN_Instance;
	protected static ref TrackingModLeaderboardPlayerData s_NJN_OwnStats;

	protected TextListboxWidget m_NJN_ItemList;
	protected TextWidget m_NJN_PointsText;
	protected TextWidget m_NJN_AmountText;
	protected ButtonWidget m_NJN_BtnMinus;
	protected ButtonWidget m_NJN_BtnPlus;
	protected ButtonWidget m_NJN_BtnBuyPvE;
	protected ButtonWidget m_NJN_BtnBuyPvP;
	protected ButtonWidget m_NJN_BtnClose;
	protected int m_NJN_Amount;

	void TrackingModShopMenu()
	{
		s_NJN_Instance = this;
		m_NJN_Amount = 1;
		NJN_SetupView();
	}

	void ~TrackingModShopMenu()
	{
		if (s_NJN_Instance == this)
			s_NJN_Instance = null;
		ForceDisableInputs(false);
	}

	static TrackingModShopMenu GetInstance()
	{
		return s_NJN_Instance;
	}

	// The server sends own stats with every leaderboard page and after every purchase.
	static void SetOwnStats(TrackingModLeaderboardPlayerData stats)
	{
		s_NJN_OwnStats = stats;
		if (s_NJN_Instance)
			s_NJN_Instance.UpdatePointsText();
	}

	// Dabs only builds the layout in the ScriptView constructor and never calls OnWidgetScriptInit on the view.
	protected void NJN_SetupView()
	{
		Widget root;

		root = GetLayoutRoot();
		if (!root)
		{
			Print("[TrackingMod] Shop layout could not be created: " + GetLayoutFile());
			return;
		}
		if (root)
		{
			m_NJN_ItemList = TextListboxWidget.Cast(root.FindAnyWidget("shop_item_list"));
			m_NJN_PointsText = TextWidget.Cast(root.FindAnyWidget("shop_points"));
			m_NJN_AmountText = TextWidget.Cast(root.FindAnyWidget("shop_amount"));
			m_NJN_BtnMinus = ButtonWidget.Cast(root.FindAnyWidget("shop_btn_minus"));
			m_NJN_BtnPlus = ButtonWidget.Cast(root.FindAnyWidget("shop_btn_plus"));
			m_NJN_BtnBuyPvE = ButtonWidget.Cast(root.FindAnyWidget("shop_btn_buy_pve"));
			m_NJN_BtnBuyPvP = ButtonWidget.Cast(root.FindAnyWidget("shop_btn_buy_pvp"));
			m_NJN_BtnClose = ButtonWidget.Cast(root.FindAnyWidget("shop_btn_close"));
		}

		RefreshItems();
		UpdatePointsText();
		ForceDisableInputs(true);
		SetFocus(root);
	}

	void RefreshItems()
	{
		TrackingModShopConfig config;
		TrackingModShopItem item;
		string rowText;
		int i;

		if (!m_NJN_ItemList)
			return;

		m_NJN_ItemList.ClearItems();
		config = g_TrackingModShopConfig;
		if (!config || !config.ShopItems)
			return;

		for (i = 0; i < config.ShopItems.Count(); i++)
		{
			item = config.ShopItems.Get(i);
			rowText = item.DisplayName + "  |  PvE: " + item.PricePvE.ToString() + "  |  PvP: " + item.PricePvP.ToString() + "  |  max " + item.MaxPerPurchase.ToString();
			m_NJN_ItemList.AddItem(rowText, null, 0);
		}

		if (m_NJN_BtnBuyPvE)
			m_NJN_BtnBuyPvE.Show(config.PointType != "PvP");
		if (m_NJN_BtnBuyPvP)
			m_NJN_BtnBuyPvP.Show(config.PointType != "PvE");
	}

	void UpdatePointsText()
	{
		if (!m_NJN_PointsText)
			return;

		if (s_NJN_OwnStats)
			m_NJN_PointsText.SetText("PvE: " + s_NJN_OwnStats.pvePoints.ToString() + " | PvP: " + s_NJN_OwnStats.pvpPoints.ToString());
	}

	protected TrackingModShopItem GetSelectedItem()
	{
		int row;

		if (!m_NJN_ItemList || !g_TrackingModShopConfig || !g_TrackingModShopConfig.ShopItems)
			return null;

		row = m_NJN_ItemList.GetSelectedRow();
		if (row < 0 || row >= g_TrackingModShopConfig.ShopItems.Count())
			return null;

		return g_TrackingModShopConfig.ShopItems.Get(row);
	}

	protected void ChangeAmount(int delta)
	{
		TrackingModShopItem item;
		int maxAmount;

		m_NJN_Amount = m_NJN_Amount + delta;
		item = GetSelectedItem();
		if (item)
		{
			maxAmount = item.MaxPerPurchase;
			if (m_NJN_Amount > maxAmount)
				m_NJN_Amount = maxAmount;
		}
		if (m_NJN_Amount < 1)
			m_NJN_Amount = 1;

		if (m_NJN_AmountText)
			m_NJN_AmountText.SetText(m_NJN_Amount.ToString());
	}

	protected void Purchase(bool usePvEPoints)
	{
		TrackingModShopItem item;
		PlayerBase player;

		item = GetSelectedItem();
		if (!item)
			return;

		player = PlayerBase.Cast(g_Game.GetPlayer());
		if (!player || !player.GetIdentity())
			return;

		GetRPCManager().SendRPC("Psyerns_Leaderboard", "PurchaseShopItem", new Param3<string, int, bool>(item.ItemClassName, m_NJN_Amount, usePvEPoints), true, player.GetIdentity());
	}

	protected void CloseShop()
	{
		g_Game.GetMission().GetHud().Show(true);
		Close();
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_NJN_BtnMinus)
		{
			ChangeAmount(-1);
			return true;
		}
		if (w == m_NJN_BtnPlus)
		{
			ChangeAmount(1);
			return true;
		}
		if (w == m_NJN_BtnBuyPvE)
		{
			Purchase(true);
			return true;
		}
		if (w == m_NJN_BtnBuyPvP)
		{
			Purchase(false);
			return true;
		}
		if (w == m_NJN_BtnClose)
		{
			CloseShop();
			return true;
		}
		return super.OnClick(w, x, y, button);
	}

	override bool OnItemSelected(Widget w, int x, int y, int row, int column, int oldRow, int oldColumn)
	{
		if (w == m_NJN_ItemList)
		{
			ChangeAmount(0);
			return true;
		}
		return super.OnItemSelected(w, x, y, row, column, oldRow, oldColumn);
	}

	protected void ForceDisableInputs(bool state)
	{
		TIntArray inputIDs;
		int i;
		int inputID;
		UAInput input;

		inputIDs = new TIntArray;
		GetUApi().GetActiveInputs(inputIDs);

		for (i = 0; i < inputIDs.Count(); i++)
		{
			inputID = inputIDs.Get(i);
			if (inputID != UAUIBack)
			{
				input = GetUApi().GetInputByID(inputID);
				if (input)
					input.ForceDisable(state);
			}
		}
	}

	override array<string> GetInputExcludes()
	{
		return {"menu"};
	}

	override bool UseMouse()
	{
		return true;
	}

	override bool CanCloseWithEscape()
	{
		return true;
	}

	override void Update(float dt)
	{
		UAInput input;

		super.Update(dt);

		input = GetUApi().GetInputByName("UAUIBack");
		if (input && input.LocalPress())
			CloseShop();
	}

	override typename GetControllerType()
	{
		return TrackingModShopController;
	}

	override string GetLayoutFile()
	{
		return "Psyerns_Leaderboard/gui/layouts/Shop/trackingModShop.layout";
	}
}
