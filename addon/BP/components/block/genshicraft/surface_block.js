export default defineComponent(({ name, template, schema }) => {
	name('genshicraft:surface_block')
	schema({
		description: 'The component for GenshiCraft blocks on surfaces without collision box.'
	})

	template(({ }, { create }) => {
		create(
			{
				'minecraft:block_light_absorption': 0,
				'minecraft:entity_collision': false,
				'minecraft:pick_collision': {
					'origin': [
						-4,
						0,
						-4
					],
					'size': [
						8,
						8,
						8
					]
				}
			},
			'minecraft:block/components'
		)
	})
})
