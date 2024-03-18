export default defineComponent(({ name, template, schema }) => {
	name('genshicraft:common_item')
	schema({
		description: 'The component for GenshiCraft common items.'
	})

	template(({ }, { create }) => {
		create(
			{
				'minecraft:render_offsets': {
					'main_hand': {
						'first_person': {
							'scale': [
								0.003,
								0.003,
								0.003
							]
						},
						'third_person': {
							'scale': [
								0.006,
								0.006,
								0.006
							]
						}
					}
				},
			},
			'minecraft:item/components'
		)
	})
})
