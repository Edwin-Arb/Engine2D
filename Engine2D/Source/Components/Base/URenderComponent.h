#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include "USceneComponent.h"

/**
 * Interface component responsible for handling visibility and 2D rendering passes.
 */
class URenderComponent : public USceneComponent
{
public:
	~URenderComponent() override = default;

	/** Toggles the component's visibility state. */
	virtual void SetVisibility(bool bNewVisibility);

	/** Sets the color properties of the underlying rendered primitive. */
	virtual void SetColor(sf::Color NewColor) = 0;

	/** Checks if this component is actively rendering to the screen. */
	bool IsVisible() const { return bIsVisible; }

	/**
	 * Resolves matrices and triggers the primitive drawing process.
	 * @param InWindow Target SFML window to render onto.
	 * @param InViewMatrix View/Camera space conversion matrix.
	 */
	void Draw(sf::RenderWindow& InWindow, const FMatrix3x3& InViewMatrix);

protected:
	/** Auto-registers this component into the owning world's render list (and removes it). */
	void OnRegister() override;
	void OnUnregister() override;

	/** Low-level rendering pipeline hook implemented by specific geometry configurations. */
	virtual void DrawShape(sf::RenderWindow& InWindow, const sf::RenderStates& InStates) = 0;

	/** Triggered automatically following updates to the component visibility configuration. */
	virtual void OnVisibilityChanged() {}

protected:
	/** Tracks the active rendering state of this component. */
	bool bIsVisible = true;
};