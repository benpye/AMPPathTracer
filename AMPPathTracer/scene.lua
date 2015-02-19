Scene = {
	Threshold = 0,
	Recursions = 10,
	SceneEmission = { 0, 0, 0 },
	Camera = {
		Fov = 1.3962634016,
		Yaw = 0,
		FocalPoint = 10,
		Aperture = 0.01,
		Origin = { 0, 0, 0 },
		Pitch = 0,
	},
	Objects = {
		{
			Type = 1,
			Point = { 0, 0, -25 },
			Normal = { 0, 0, 1 },
			Properties = {
				SpecularColor = { 1, 1, 1 },
				DiffuseColor = { 0.75, 0.75, 0.75 },
				RefractiveIndex = 1,
				Emission = { 0, 0, 0 },
				Transmittance = 0,
				Reflectivity = 0,
				Roughness = 0,
			},
		},
		{
			Type = 1,
			Point = { 0, 0, 25 },
			Normal = { 0, 0, -1 },
			Properties = {
				SpecularColor = { 1, 1, 1 },
				DiffuseColor = { 0.75, 0.75, 0.75 },
				RefractiveIndex = 1,
				Emission = { 0, 0, 0 },
				Transmittance = 0,
				Reflectivity = 0,
				Roughness = 0,
			},
		},
		{
			Type = 1,
			Point = { 0, 100, 0 },
			Normal = { 0, -1, 0 },
			Properties = {
				SpecularColor = { 1, 1, 1 },
				DiffuseColor = { 0.75, 0.75, 0.75 },
				RefractiveIndex = 1,
				Emission = { 0, 0, 0 },
				Transmittance = 0,
				Reflectivity = 0,
				Roughness = 0,
			},
		},
		{
			Type = 1,
			Point = { 0, -5, 0 },
			Normal = { 0, 1, 0 },
			Properties = {
				SpecularColor = { 1, 1, 1 },
				DiffuseColor = { 0.75, 0.75, 0.75 },
				RefractiveIndex = 1,
				Emission = { 0, 0, 0 },
				Transmittance = 0,
				Reflectivity = 0,
				Roughness = 0,
			},
		},
		{
			Type = 1,
			Point = { 25, 0, 0 },
			Normal = { -1, 0, 0 },
			Properties = {
				SpecularColor = { 1, 1, 1 },
				DiffuseColor = { 0.25, 0.75, 0.25 },
				RefractiveIndex = 1,
				Emission = { 0, 0, 0 },
				Transmittance = 0,
				Reflectivity = 0,
				Roughness = 0,
			},
		},
		{
			Type = 1,
			Point = { -25, 0, 0 },
			Normal = { 1, 0, 0 },
			Properties = {
				SpecularColor = { 1, 1, 1 },
				DiffuseColor = { 0.75, 0.25, 0.25 },
				RefractiveIndex = 1,
				Emission = { 0, 0, 0 },
				Transmittance = 0,
				Reflectivity = 0,
				Roughness = 0,
			},
		},
		{
			Type = 0,
			Radius = 500,
			Origin = { 0, 60, 524.9 },
			Properties = {
				SpecularColor = { 1, 1, 1 },
				DiffuseColor = { 0.75, 0.75, 0.75 },
				RefractiveIndex = 1,
				Emission = { 4, 4, 4 },
				Transmittance = 0,
				Reflectivity = 0,
				Roughness = 0,
			},
		},
		{
			Type = 0,
			Radius = 10,
			Origin = { 12, 65, -15 },
			Properties = {
				SpecularColor = { 1, 1, 1 },
				DiffuseColor = { 0.75, 0.75, 0.75 },
				RefractiveIndex = 1.4,
				Emission = { 0, 0, 0 },
				Transmittance = 1,
				Reflectivity = 1,
				Roughness = 0,
			},
		},
		{
			Type = 0,
			Radius = 10,
			Origin = { -12, 80, -15 },
			Properties = {
				SpecularColor = { 0.9, 0.75, 0 },
				DiffuseColor = { 0.75, 0.75, 0.75 },
				RefractiveIndex = 50,
				Emission = { 0, 0, 0 },
				Transmittance = 0,
				Reflectivity = 1,
				Roughness = 0.1,
			},
		},
	},
}

return Scene